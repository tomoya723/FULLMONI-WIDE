/*
 * fan_tacho.c — Prodrive-style fan/sweep tachometer renderer
 *
 * Draws a fan-shaped tachometer (segmented blocks in a sweep pattern)
 * on an LVGL container using the post-draw event.
 *
 * Geometry:
 *   The sweep centre is placed below the visible area. Columns of small
 *   rectangular segments radiate outward from this centre, producing the
 *   characteristic "opening fan" look.
 *
 *   0 RPM: lower-left   →   9000 RPM: upper-right
 *
 * Rendering is done with lv_draw_rect on rotated coordinates,
 * calculated per-segment using fixed-point trigonometry (LVGL's
 * lv_trigo_sin / lv_trigo_cos, which return values × 32768).
 */

#include "fan_tacho.h"
#include <lvgl/lvgl.h>

/* ── Configuration ─────────────────────────────────────────────────────── */

/* Fan geometry – all coordinates relative to the dashboard container */
/* eez003 places the tachometer on the right side of the 765x256 dashboard. */
#define FAN_CX          365     /* Sweep centre X (aligned to right tachometer area) */
#define FAN_CY          420     /* Sweep centre Y (below visible area to create fan shape) */
#define FAN_R_INNER     250     /* Inner radius where segments start */

/* Angular sweep (degrees × 10, LVGL convention) */
/* Using 0° = 3-o'clock, angles increase CW.
 * We want the fan to go from ~305° (= lower-left, 0 RPM)
 * to ~230° (= upper-right, 9000 RPM).
 * In our mapping: start_deg = 305, end_deg = 230
 * So the sweep goes CW from 305° → 360° → 230° = 285° total.
 * But we'll use a simple linear interpolation and our own trig. */

/* We use math-convention angles: 0° = right, CCW positive.
 * Start (0 RPM)    = 120°  (upper-left quadrant)
 * End   (9000 RPM) =  42°  (upper-right quadrant)
 * Sweep direction = CW (decreasing angle) */
#define FAN_ANGLE_START_DEG  118   /* 0 RPM */
#define FAN_ANGLE_END_DEG     42   /* 9000 RPM */

/* Segment geometry */
#define SEG_WIDTH        7     /* Angular width of each segment block (px) */
#define SEG_HEIGHT       12    /* Radial height of each segment block (px) */
#define SEG_GAP_RADIAL   2     /* Gap between blocks in same column (px) */
#define COL_GAP          1     /* Gap between columns (in column-index terms) */

/* Columns: one column per 200 RPM → 45 columns for 0–9000 */
#define NUM_COLUMNS      45
#define RPM_PER_COL      200

/* Number of segments per column increases with RPM.
 * Min at 0 RPM, max at 9000 RPM. */
#define SEG_MIN          4     /* Segments at lowest RPM column */
#define SEG_MAX          11    /* Segments at highest RPM column */

/* RPM number labels on the sweep */
static const uint32_t rpm_labels[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
#define NUM_RPM_LABELS   9

/* ── State ─────────────────────────────────────────────────────────────── */
static uint32_t s_current_rpm = 0;
static uint32_t s_redline_rpm = 7000;
static lv_obj_t *s_container  = NULL;

/* ── Colors ────────────────────────────────────────────────────────────── */
#define COL_SEG_ACTIVE   0x00E0FF   /* Cyan */
#define COL_SEG_REDLINE  0xFF0000   /* Red */
#define COL_SEG_INACTIVE 0x0A1520   /* Very dark (barely visible) */

/* ── Trig helpers (float, simpler than fixed-point for this use case) ──── */
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline float deg_to_rad(float deg) { return deg * (float)(M_PI / 180.0); }

/* ── Drawing callback ──────────────────────────────────────────────────── */

static void fan_tacho_draw_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_DRAW_POST_BEGIN) return;

    lv_obj_t *obj = lv_event_get_target(e);
    lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);

    /* Get the container's screen-space coordinates */
    lv_area_t obj_area;
    lv_obj_get_coords(obj, &obj_area);
    int ox = obj_area.x1;
    int oy = obj_area.y1;

    /* Centre of fan arc in screen coordinates */
    int cx = ox + FAN_CX;
    int cy = oy + FAN_CY;

    /* Angle step per column */
    float angle_span = (float)(FAN_ANGLE_START_DEG - FAN_ANGLE_END_DEG);
    float angle_step = angle_span / (float)(NUM_COLUMNS - 1);

    for (int col = 0; col < NUM_COLUMNS; col++) {
        /* Column angle (math convention: 0=right, CCW positive) */
        float col_angle_deg = (float)FAN_ANGLE_START_DEG - col * angle_step;
        float col_angle_rad = deg_to_rad(col_angle_deg);
        float cos_a = cosf(col_angle_rad);
        float sin_a = sinf(col_angle_rad);

        /* Number of segments for this column (linear interpolation) */
        float t = (float)col / (float)(NUM_COLUMNS - 1);
        int num_segs = SEG_MIN + (int)(t * (float)(SEG_MAX - SEG_MIN) + 0.5f);

        /* RPM value represented by this column */
        uint32_t col_rpm = (uint32_t)(col * RPM_PER_COL);

        for (int seg = 0; seg < num_segs; seg++) {
            /* Radial distance from centre for this segment */
            float r = (float)FAN_R_INNER + (float)seg * (float)(SEG_HEIGHT + SEG_GAP_RADIAL);

            /* Centre of this segment in screen coords */
            float seg_cx = (float)cx + r * cos_a;
            /* Screen Y is inverted (down = positive) */
            float seg_cy = (float)cy - r * sin_a;

            /* Choose color */
            uint32_t color;
            if (col_rpm <= s_current_rpm) {
                color = (col_rpm >= s_redline_rpm) ? COL_SEG_REDLINE : COL_SEG_ACTIVE;
            } else {
                color = COL_SEG_INACTIVE;
            }

            /* Draw axis-aligned rectangle at the radially-placed segment centre.
             * The "fan" visual comes from radial placement along different angles. */
            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_color = lv_color_hex(color);
            rect_dsc.bg_opa = LV_OPA_COVER;
            rect_dsc.border_width = 0;
            rect_dsc.radius = 0;

            {
                lv_area_t seg_area;
                seg_area.x1 = (lv_coord_t)(seg_cx - SEG_WIDTH / 2);
                seg_area.y1 = (lv_coord_t)(seg_cy - SEG_HEIGHT / 2);
                seg_area.x2 = seg_area.x1 + SEG_WIDTH - 1;
                seg_area.y2 = seg_area.y1 + SEG_HEIGHT - 1;
                lv_draw_rect(draw_ctx, &rect_dsc, &seg_area);
            }
        }
    }

    /* ── RPM number labels along the sweep ──────────────────────────────── */
    for (int i = 0; i < NUM_RPM_LABELS; i++) {
        uint32_t rpm_val = rpm_labels[i] * 1000;
        float t_label = (float)(rpm_val) / 9000.0f;
        float label_angle_deg = (float)FAN_ANGLE_START_DEG -
                                t_label * (float)(FAN_ANGLE_START_DEG - FAN_ANGLE_END_DEG);
        float label_angle_rad = deg_to_rad(label_angle_deg);

        /* Place label just inside the inner radius */
        float lr = (float)(FAN_R_INNER - 18);
        float lx = (float)cx + lr * cosf(label_angle_rad);
        float ly = (float)cy - lr * sinf(label_angle_rad);

        char buf[4];
        lv_snprintf(buf, sizeof(buf), "%u", (unsigned)rpm_labels[i]);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_white();
        label_dsc.font = LV_FONT_DEFAULT;
        label_dsc.opa = LV_OPA_COVER;

        lv_area_t label_area;
        label_area.x1 = (lv_coord_t)(lx - 8);
        label_area.y1 = (lv_coord_t)(ly - 7);
        label_area.x2 = label_area.x1 + 16;
        label_area.y2 = label_area.y1 + 14;

        lv_draw_label(draw_ctx, &label_dsc, &label_area, buf, NULL);
    }
}

/* ── Public API ────────────────────────────────────────────────────────── */

void fan_tacho_init(lv_obj_t *container)
{
    if (!container) return;
    s_container = container;
    lv_obj_add_event_cb(container, fan_tacho_draw_cb, LV_EVENT_DRAW_POST_BEGIN, NULL);
}

void fan_tacho_set_rpm(uint32_t rpm)
{
    if (rpm > 9000) rpm = 9000;
    if (rpm != s_current_rpm) {
        s_current_rpm = rpm;
        if (s_container) {
            lv_obj_invalidate(s_container);
        }
    }
}

void fan_tacho_set_redline(uint32_t rpm)
{
    s_redline_rpm = rpm;
}
