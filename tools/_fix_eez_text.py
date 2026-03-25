"""
Modify fullmoni.eez-project to match aw002 AppWizard layout.
Changes text content, colors, and sizes for specific widgets.
Background images are NOT changed (user will do manually).
"""
import json
import copy

PROJECT = 'Firmware/eez/fullmoni.eez-project'

with open(PROJECT, 'r', encoding='utf-8') as f:
    proj = json.load(f)

page = proj['userPages'][0]

# Define modifications: widget_name -> dict of changes
# Each change can modify: text, width, height, left, top, and style overrides
MODS = {
    # 1. Speed unit: "kmh" -> "km/h"
    'ui_LblSPDUnit': {
        'text': 'km/h',
    },
    # 2. Trip label: "TRIP" -> "Trip:"
    'ui_LblTripName': {
        'text': 'Trip:',
        'width': 32,
    },
    # 3. ODO label: "ODO" -> "Total:"
    'ui_LblODOName': {
        'text': 'Total:',
        'width': 40,
    },
    # 4. Trip value color: white -> #C8C8C8
    'ui_LblTrip': {
        'style': {'text_color': '#C8C8C8'},
    },
    # 5. ODO value color: white -> #C8C8C8
    'ui_LblODO': {
        'style': {'text_color': '#C8C8C8'},
    },
    # 6. Trip name/unit color
    'ui_LblTripUnit': {
        'style': {'text_color': '#C8C8C8'},
    },
    'ui_LblODOUnit': {
        'style': {'text_color': '#C8C8C8'},
    },
    # 7. TIME color: white -> #C0C0C0
    'ui_LblTIME': {
        'style': {'text_color': '#C0C0C0'},
    },
    # 8. Trip/ODO name colors
    # (already covered by text changes above, add color too)
}
# Also set trip/odo name colors
for name in ['ui_LblTripName', 'ui_LblODOName']:
    if 'style' not in MODS[name]:
        MODS[name]['style'] = {}
    MODS[name]['style']['text_color'] = '#C8C8C8'


def apply_mods(components):
    count = 0
    for c in components:
        wname = c.get('name', '') or c.get('identifier', '')
        if wname in MODS:
            mod = MODS[wname]
            # Text
            if 'text' in mod:
                old = c.get('text', '')
                c['text'] = mod['text']
                print(f"  {wname}: text '{old}' -> '{mod['text']}'")
            # Position/Size
            for key in ('left', 'top', 'width', 'height'):
                if key in mod:
                    old = c.get(key)
                    c[key] = mod[key]
                    print(f"  {wname}: {key} {old} -> {mod[key]}")
            # Style overrides
            if 'style' in mod:
                ls = c.get('localStyles', {})
                defn = ls.get('definition', {})
                main = defn.get('MAIN', {})
                default = main.get('DEFAULT', {})
                for sk, sv in mod['style'].items():
                    old = default.get(sk, '(none)')
                    default[sk] = sv
                    print(f"  {wname}: style.{sk} {old} -> {sv}")
                main['DEFAULT'] = default
                defn['MAIN'] = main
                ls['definition'] = defn
                c['localStyles'] = ls
            count += 1
        # Recurse
        children = c.get('children', [])
        if children:
            count += apply_mods(children)
    return count

print("Applying modifications...")
n = apply_mods(page['components'])
print(f"\nModified {n} widgets.")

# Save
with open(PROJECT, 'w', encoding='utf-8') as f:
    json.dump(proj, f, indent=2, ensure_ascii=False)
print(f"Saved to {PROJECT}")
