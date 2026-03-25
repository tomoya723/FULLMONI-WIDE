#!/usr/bin/env python3
"""Dump EEZ project font definitions and per-widget font assignments."""
import json

with open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8') as f:
    proj = json.load(f)

print('=== EEZ Fonts defined ===')
for font in proj.get('fonts', []):
    src = font.get('source', {})
    rf = font.get('renderingFont', {})
    print(f"  {font.get('name', '?'):25s} file={src.get('filePath', '?')} size={rf.get('size', '?')} bpp={font.get('bpp', '?')}")

print()
print('=== EEZ Widget font usage ===')

def walk(widgets, depth=0):
    for w in widgets:
        ident = w.get('identifier', '?')
        wtype = w.get('type', '?')
        local = w.get('localStyles', {})
        font = None
        for state_name, state_styles in local.items():
            if isinstance(state_styles, dict):
                for part_name, part_styles in state_styles.items():
                    if isinstance(part_styles, dict):
                        tf = part_styles.get('text_font')
                        if tf:
                            font = tf
        use_style = w.get('useStyle', '')
        if font or wtype in ('LVGLLabelWidget', 'LVGLBarWidget'):
            print(f"  {ident:25s} {wtype:25s} font={font if font else '(inherited)'} useStyle={use_style if use_style else '(none)'}")
        if 'children' in w:
            walk(w['children'], depth + 1)

screen = proj['userPages'][0]['components'][0]
walk(screen['children'])
