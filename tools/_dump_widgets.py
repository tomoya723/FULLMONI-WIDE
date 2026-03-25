import json, sys

with open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8') as f:
    j = json.load(f)

page = j['userPages'][0]

targets = [
    'ui_LblSPDUnit', 'ui_LblTripName', 'ui_LblODOName', 'ui_LblRPMUnit',
    'ui_LblTrip', 'ui_LblODO', 'ui_LblTIME', 'ui_LblSPD',
    'ui_LblTripUnit', 'ui_LblODOUnit', 'ui_LblEmpty', 'ui_LblFull',
    'ui_LblGEAR', 'ui_LblRPM', 'ui_BarFUEL',
]

def find_all(components):
    for c in components:
        name = c.get('name', '') or c.get('identifier', '')
        if name in targets:
            ls = c.get('localStyles', {})
            d = ls.get('definition', {}).get('MAIN', {}).get('DEFAULT', {}) if ls else {}
            left = c.get('left', '-')
            top = c.get('top', '-')
            w = c.get('width', '-')
            h = c.get('height', '-')
            text = c.get('text', '')
            print(f"{name}:")
            print(f"  pos=({left},{top}), size=({w}x{h})")
            print(f"  text={repr(text)}")
            print(f"  styles={d}")
            print()
        children = c.get('children', [])
        if children:
            find_all(children)

find_all(page['components'])
