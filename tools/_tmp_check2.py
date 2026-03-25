import json

with open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8') as f:
    proj = json.load(f)

print('=== FONT DEFINITIONS ===')
for font in proj.get('fonts', []):
    name = font.get('name', '?')
    src = font.get('source', {})
    print(f'  {name}: file={src.get("filePath","?")}, size={src.get("size","?")}')

print()
print('=== WIDGET FONT ASSIGNMENTS ===')

def walk(obj, depth=0):
    name = obj.get('name', '')
    objtype = obj.get('type', '')
    text = obj.get('text', '')
    ident = obj.get('identifier', '')

    # Search for text_font in localStyles
    ls = obj.get('localStyles', {})
    font_found = None
    def search_font(d):
        nonlocal font_found
        if isinstance(d, dict):
            if 'text_font' in d:
                font_found = d['text_font']
            for v in d.values():
                search_font(v)
    search_font(ls)

    display_name = ident or name or objtype
    if 'LVGLLabel' in objtype or 'Lbl' in display_name:
        marker = f'font={font_found}' if font_found else 'NO FONT'
        t = f' text="{text}"' if text else ''
        print(f'  {display_name}: {marker}{t}')

    # Recurse into components (not children)
    for child in obj.get('components', []):
        walk(child, depth+1)
    for child in obj.get('children', []):
        walk(child, depth+1)

for page in proj.get('userPages', []):
    pname = page.get('name', '?')
    comps = page.get('components', [])
    print(f'--- Page: {pname} ({len(comps)} components) ---')
    for comp in comps:
        walk(comp)
