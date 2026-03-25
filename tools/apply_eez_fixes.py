"""Apply all aw002 coordinate/size differences to EEZ project (phase 2)"""
import json

EEZ_PATH = 'Firmware/eez/fullmoni.eez-project'

with open(EEZ_PATH, 'r', encoding='utf-8') as f:
    proj = json.load(f)

page = proj['userPages'][0]
root = page['components'][0]
dash = root['children'][0]  # ui_ContainerDashboard
children = dash['children']

changes = []

# Build index by identifier
idx = {w.get('identifier', ''): (i, w) for i, w in enumerate(children)}

# --- Widget size/position corrections to match aw002 ---
# Format: identifier -> { field: new_value }
# Computed from aw002 absolute coords with dashboard center_x=418, center_y=128
fixes = {
    'ui_LblAFR': {'left': 115, 'top': 85, 'width': 130, 'height': 39},
    'ui_LblTIME': {'top': -113, 'width': 94, 'height': 31},
    'ui_LblRPM': {'width': 100, 'height': 28},
    'ui_LblSPD': {'left': 271, 'width': 100, 'height': 28},
    'ui_LblTrip': {'left': 325, 'width': 50, 'height': 20},
    'ui_LblODO': {'width': 60, 'height': 20},
    'ui_LblBattery': {'height': 32},
}

for name, props in fixes.items():
    if name in idx:
        _, w = idx[name]
        for field, new_val in props.items():
            old_val = w.get(field, 0)
            if old_val != new_val:
                w[field] = new_val
                changes.append(f'{name}: {field} {old_val} -> {new_val}')

# --- Write back ---
with open(EEZ_PATH, 'w', encoding='utf-8') as f:
    json.dump(proj, f, indent=2, ensure_ascii=False)

print('Applied changes:')
for c in changes:
    print(f'  - {c}')
print(f'\nTotal: {len(changes)} changes')
