#!/usr/bin/env python3
"""Parse an emWin AppWizard .AppWizard file and dump object tree with dimensions.
Reads UTF-16 LE format (.AppWizard files saved by emWin).
Output: structured text with display settings, variables, screens, and objects.
"""
import re, sys, os


def parse_appwizard(path):
    with open(path, "r", encoding="utf-16", errors="replace") as f:
        lines = [l.rstrip() for l in f]

    out = []
    def pr(s=""):
        out.append(s)

    # --- Global settings ---
    display = {}
    for l in lines:
        m = re.match(r"\s*(xSizeDisplay|ySizeDisplay|ColorConv|MultibufEnable|MinTimePerFrame)\s*=\s*(.+)", l)
        if m:
            display[m.group(1)] = m.group(2)

    # --- Variables ---
    variables = []
    in_vars = False
    for l in lines:
        if "<Variables>" in l:
            in_vars = True; continue
        if "</Variables>" in l:
            in_vars = False; continue
        if in_vars:
            m = re.match(r"\s*Item=(\d+),(\w+),(\d+)", l)
            if m:
                variables.append((int(m.group(1)), m.group(2), int(m.group(3))))

    # --- Animations ---
    animations = []
    in_anim = False
    for l in lines:
        if "<Animations>" in l:
            in_anim = True; continue
        if "</Animations>" in l:
            in_anim = False; continue
        if in_anim:
            m = re.match(r"\s*Item=(\d+),(\w+),(.*)", l)
            if m:
                animations.append((int(m.group(1)), m.group(2), m.group(3)))

    # --- Parse Objects section: sequential screen grouping ---
    obj_start = obj_end = None
    for i, l in enumerate(lines):
        if "<Objects>" in l and obj_start is None:
            obj_start = i + 1
        if "</Objects>" in l:
            obj_end = i
            break

    screens = []  # list of (screen_props_dict, [obj_dicts])
    current_screen = None
    current_objects = []

    def read_dispose(start):
        """Read a <Dispose>...</Dispose> block, return (props_dict, next_line_index)."""
        props = {}
        j = start
        while j < obj_end:
            sl = lines[j].strip()
            if sl == "</Dispose>":
                return props, j + 1
            km = re.match(r"(\w+)=(.*)", sl)
            if km:
                props[km.group(1)] = km.group(2)
            # Sub-block like <X0>
            sm = re.match(r"<(\w+)>", sl)
            if sm and sm.group(1) in ("X0", "Y0", "X1", "Y1"):
                tag = sm.group(1)
                j += 1
                while j < obj_end and lines[j].strip() != f"</{tag}>":
                    pm = re.match(r"(\w+)=(.*)", lines[j].strip())
                    if pm:
                        props[f"{tag}_{pm.group(1)}"] = pm.group(2)
                    j += 1
            j += 1
        return props, j

    def read_sub_block(start, tag):
        """Read a <Tag>...</Tag> sub-block, return (props_dict, next_line_index)."""
        props = {}
        j = start
        while j < obj_end:
            sl = lines[j].strip()
            if sl == f"</{tag}>":
                return props, j + 1
            km = re.match(r"(\w+)=(.*)", sl)
            if km:
                props[km.group(1)] = km.group(2)
            j += 1
        return props, j

    if obj_start and obj_end:
        i = obj_start
        while i < obj_end:
            l = lines[i].strip()

            # Screen opening
            if l == "<Screen>":
                if current_screen is not None:
                    screens.append((current_screen, current_objects))
                current_screen = {}
                current_objects = []
                i += 1
                while i < obj_end:
                    sl = lines[i].strip()
                    if sl == "</Screen>":
                        i += 1; break
                    if sl == "<Dispose>":
                        dp, i = read_dispose(i + 1)
                        for k, v in dp.items():
                            current_screen[f"d_{k}"] = v
                        continue
                    km = re.match(r"(\w+)=(.*)", sl)
                    if km:
                        current_screen[km.group(1)] = km.group(2)
                    i += 1
                continue

            # Object opening (non-Screen)
            om = re.match(r"<(\w+)>", l)
            if om:
                tag = om.group(1)
                obj = {"_type": tag}
                i += 1
                while i < obj_end:
                    ol = lines[i].strip()
                    if ol == f"</{tag}>":
                        i += 1; break
                    if ol == "<Dispose>":
                        dp, i = read_dispose(i + 1)
                        for k, v in dp.items():
                            obj[f"d_{k}"] = v
                        continue
                    # Nested sub-block
                    sm = re.match(r"<(\w+)>", ol)
                    if sm:
                        stag = sm.group(1)
                        sp, i = read_sub_block(i + 1, stag)
                        for k, v in sp.items():
                            obj[f"{stag}.{k}"] = v
                        continue
                    km = re.match(r"(\w+)=(.*)", ol)
                    if km:
                        obj[km.group(1)] = km.group(2)
                    i += 1
                current_objects.append(obj)
                continue

            i += 1

        if current_screen is not None:
            screens.append((current_screen, current_objects))

    # --- Interactions ---
    interactions = []
    in_inter = False
    current_inter = {}
    for l in lines:
        ls = l.strip()
        if "<Interactions>" in ls:
            in_inter = True; continue
        if "</Interactions>" in ls:
            in_inter = False; continue
        if in_inter:
            if ls.startswith("IdSrc="):
                current_inter["IdSrc"] = ls[6:]
            elif ls.startswith("Slot="):
                current_inter["Slot"] = ls[5:]
                interactions.append(current_inter)
                current_inter = {}
            elif "<SWAPSCREEN>" in ls:
                current_inter["Swap"] = True

    # ==== OUTPUT ====
    pr("=" * 80)
    pr(f"emWin AppWizard Object Dump: {os.path.basename(path)}")
    pr("=" * 80)
    pr()
    pr("## Display Settings")
    for k, v in display.items():
        pr(f"  {k} = {v}")
    pr()

    pr("## Variables")
    pr(f"  {'ID':>5}  {'Name':<20}  {'Default':>7}")
    pr(f"  {'─'*5}  {'─'*20}  {'─'*7}")
    for vid, vname, vdef in sorted(variables):
        pr(f"  {vid:>5}  {vname:<20}  {vdef:>7}")
    pr()

    if animations:
        pr("## Animations")
        for aid, aname, aparams in animations:
            pr(f"  {aid:>5}  {aname:<20}  params={aparams}")
        pr()

    pr("## Screens & Objects")
    pr()

    # Object types that are actual widgets (not set-property commands)
    widget_types = {"Image", "Text", "Gauge", "Rotary", "Progbar", "Timer",
                    "Slider", "Button", "Window", "Box", "Multiedit"}

    for idx, (scr, objs) in enumerate(screens):
        sname = scr.get("IdString", f"Screen_{idx}")
        xs = scr.get("d_xSize", "auto")
        ys = scr.get("d_ySize", "auto")
        x0_off = scr.get("d_X0_Off", "0")
        y0_off = scr.get("d_Y0_Off", "0")
        root = scr.get("RootId", "?")

        pr(f"### {sname}")
        pr(f"    Size: {xs} x {ys}  |  Position: X0={x0_off}, Y0={y0_off}  |  RootId={root}")
        pr()

        widgets = [o for o in objs if o["_type"] in widget_types]
        if not widgets:
            pr("    (no widget objects)")
            pr()
            continue

        pr(f"    {'Type':<10} {'IdString':<28} {'Size':>11} {'X0_Off':>7} {'Y0_Off':>7}  Properties")
        pr(f"    {'─'*10} {'─'*28} {'─'*11} {'─'*7} {'─'*7}  {'─'*40}")

        for o in widgets:
            typ = o["_type"]
            idstr = o.get("IdString", "")
            xs = o.get("d_xSize", "?")
            ys = o.get("d_ySize", "?")
            x0 = o.get("d_X0_Off", "")
            y0 = o.get("d_Y0_Off", "")
            size_str = f"{xs:>5}x{ys:<5}"

            extras = []
            skip_keys = {"_type", "IdString", "Id", "IdPrev", "IdPost", "ParentId",
                         "d_xSize", "d_ySize", "d_X0_Off", "d_Y0_Off",
                         "d_X0_Mode", "d_Y0_Mode", "d_X1_Off", "d_Y1_Off",
                         "d_X0_Id", "d_Y0_Id", "d_X1_Mode", "d_Y1_Mode", "d_X1_Id", "d_Y1_Id"}
            for k in sorted(o.keys()):
                if k in skip_keys:
                    continue
                extras.append(f"{k}={o[k]}")
            extra_str = ", ".join(extras[:5])

            pr(f"    {typ:<10} {idstr:<28} {size_str} {x0:>7} {y0:>7}  {extra_str}")

        pr()

    pr("## Interactions (Variable → Widget Slots)")
    pr()
    by_src = {}
    for inter in interactions:
        src = inter.get("IdSrc", "?")
        by_src.setdefault(src, []).append(inter)
    for src, slots in by_src.items():
        pr(f"  [{src}]")
        for s in slots:
            swap = " [SWAP]" if s.get("Swap") else ""
            pr(f"    → {s.get('Slot', '?')}{swap}")
        pr()

    return "\n".join(out)


if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.environ["TEMP"], "aw002.AppWizard")
    outpath = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.environ["TEMP"], "aw002_dump.txt")
    text = parse_appwizard(path)
    with open(outpath, "w", encoding="utf-8") as f:
        f.write(text)
    print(f"Done. Output written to {outpath}")
