#!/usr/bin/env python3
"""Dump current eez-project font definitions and widget font assignments."""
import json, sys

with open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8') as f:
    proj = json.load(f)

print('=== FONT DEFINITIONS ===')
for font in proj.get('fonts', []):
    name = font.get('name', '?')
    src = font.get('source', {})
    fp = src.get('filePath', '?')
    sz = src.get('size', '?')
    print(f'  {name}: file={fp}, size={sz}')

print()

# Show page structure summary
print('=== PAGE STRUCTURE ===')
for page in proj.get('userPages', []):
    pname = page.get('name', '?')
    children = page.get('children', [])
    widgets = page.get('widgets', [])
    print(f'  Page: {pname}, children={len(children)}, widgets={len(widgets)}')
    for c in children[:3]:
        print(f'    child: {c.get("name","?")} ({c.get("type","?")}), children={len(c.get("children",[]))}')
    for w in widgets[:3]:
        print(f'    widget: {w.get("name","?")} ({w.get("type","?")})')

print()
print('=== WIDGET FONT ASSIGNMENTS ===')

def find_widgets(obj, path=''):
    name = obj.get('name', '')
    objtype = obj.get('type', '')
    text = obj.get('text', '')
    ls = obj.get('localStyles', {})

    # Search all nested dicts for text_font
    font_found = None
    def search_font(d):
        nonlocal font_found
        if isinstance(d, dict):
            if 'text_font' in d:
                font_found = d['text_font']
            for v in d.values():
                search_font(v)
    search_font(ls)

    # Show all widgets that have text or are Labels
    if 'Label' in objtype or 'Lbl' in name:
        marker = f'text_font={font_found}' if font_found else 'NO FONT SET'
        t = f'  text="{text}"' if text else ''
        print(f'  {name} ({objtype}): {marker}{t}')
        if ls:
            defn = ls.get('definition', {})
            for k1, v1 in defn.items():
                if isinstance(v1, dict):
                    for k2, v2 in v1.items():
                        if isinstance(v2, dict):
                            print(f'    localStyles.definition.{k1}.{k2} = {list(v2.keys())}')

    for child in obj.get('children', []):
        find_widgets(child, path + '/' + name)

for page in proj.get('userPages', []):
    pname = page.get('name', '?')
    print(f'--- Page: {pname} ---')
    find_widgets(page)
