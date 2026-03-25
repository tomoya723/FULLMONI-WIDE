#!/usr/bin/env python3
"""Check current font definitions and widget font assignments."""
import json

proj = json.load(open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8'))

print('=== Font definitions ===')
for f in proj.get('fonts', []):
    src = f.get('source', {})
    name = f['name']
    fpath = src.get('filePath', '?')
    size = src.get('size', '?')
    print(f'  {name:25s} file={fpath:35s} size={size}')

print()
print('=== Widget font assignments ===')
screen = proj['userPages'][0]['components'][0]

def walk(widgets):
    for w in widgets:
        ls = w.get('localStyles', {})
        defn = ls.get('definition', {})
        main = defn.get('MAIN', {})
        default = main.get('DEFAULT', {})
        text_font = default.get('text_font')
        ident = w.get('identifier', '?')
        wtype = w.get('type', '?')
        if text_font or 'Label' in wtype:
            print(f'  {ident:25s} {wtype:25s} text_font={text_font if text_font else "(none)"}')
        for child in w.get('children', []):
            walk([child])

walk(screen['children'])
