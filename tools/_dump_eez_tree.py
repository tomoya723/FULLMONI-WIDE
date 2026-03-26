import json
with open(r'C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003\eez003.eez-project','r') as f:
    p = json.load(f)
def dump(node, depth=0):
    ident = node.get('identifier','(screen)')
    t = node.get('type','?')
    l = node.get('left', 0)
    tp = node.get('top', 0)
    w = node.get('width', 0)
    h = node.get('height', 0)
    ls = node.get('localStyles', {})
    defn = ls.get('definition', {})
    main_def = defn.get('MAIN', {}).get('DEFAULT', {})
    align = main_def.get('align', 'TOP_LEFT')
    indent = '  ' * depth
    print(f"{indent}{t} '{ident}' pos=({l},{tp}) size={w}x{h} align={align}")
    for c in node.get('children', []):
        dump(c, depth + 1)
screen = p['userPages'][0]['components'][0]
dump(screen)
