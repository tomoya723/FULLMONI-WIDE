import json

with open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8') as f:
    proj = json.load(f)

page = proj['userPages'][0]
root = page['components'][0]

def get_styles(w):
    ls = w.get('localStyles', {})
    defn = ls.get('definition', {}).get('MAIN', {}).get('DEFAULT', {}) if ls else {}
    return {k: v for k, v in defn.items() if v}

# Dashboard container center (with CENTER align from 800x256 screen)
# center_x = 400 + container_left = 400 + 18 = 418
# center_y = 128 + container_top = 128 + 0 = 128
CX = 418
CY = 128

dash = root['children'][0]
print(f"Dashboard center: ({CX}, {CY})")
print(f"{'Name':<20} {'Offset':<18} {'Size':<10} {'Abs Left':>9} {'Abs Top':>8} {'Abs Right':>10} {'Abs Bot':>8}")
print("-" * 95)

for sub in dash.get('children', []):
    name = sub.get('identifier', '?')
    ox = sub.get('left', 0)
    oy = sub.get('top', 0)
    w = sub.get('width', 0)
    h = sub.get('height', 0)
    abs_left = CX + ox - w/2
    abs_top = CY + oy - h/2
    abs_right = abs_left + w
    abs_bot = abs_top + h
    text = sub.get('text', '')
    extra = f'  "{text}"' if text else ''
    print(f"{name:<20} ({ox:>4},{oy:>4})    {w:>3}x{h:<3}   {abs_left:>8.1f} {abs_top:>8.1f} {abs_right:>9.1f} {abs_bot:>8.1f}{extra}")
