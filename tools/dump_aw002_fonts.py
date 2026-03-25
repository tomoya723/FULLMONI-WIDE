#!/usr/bin/env python3
"""Extract widget-font mapping from aw002.AppWizard via git show."""
import subprocess, re

raw = subprocess.check_output(['git','show','remotes/origin/main:Firmware/aw002/aw002.AppWizard'])
data = raw.decode('utf-8')

# Parse objects by screen
screen_name = '?'
in_screen = False
obj_stack = []
objects = []

for line in data.split('\n'):
    s = line.strip()
    if s == '<Screen>':
        in_screen = True
        continue
    if s == '</Screen>':
        in_screen = False
        screen_name = '?'
        continue
    m = re.match(r'IdString=(ID_SCREEN_\w+)', s)
    if m and in_screen:
        screen_name = m.group(1)
        continue
    for tag in ['Text', 'Number', 'Slider', 'Rotary', 'ProgBar']:
        if s == f'<{tag}>':
            obj_stack.append({'screen': screen_name, 'type': tag, 'id': '?'})
            break
        if s == f'</{tag}>' and obj_stack:
            objects.append(obj_stack.pop())
            break
    if obj_stack:
        m = re.match(r'IdString=(ID_\w+)', s)
        if m:
            obj_stack[-1]['id'] = m.group(1)
        m2 = re.match(r'Font=(.*\.xbf)', s)
        if m2:
            obj_stack[-1]['font'] = m2.group(1)

# Parse Interactions to get SetFont
interactions = []
in_inter = False
inter = {}
for line in data.split('\n'):
    s = line.strip()
    if s == '<Interaction>':
        in_inter = True
        inter = {}
        continue
    if s == '</Interaction>':
        in_inter = False
        if 'receiver' in inter and 'font' in inter:
            interactions.append(inter)
        continue
    if in_inter:
        m = re.match(r'<ReceiverIdTxt>(.*?)</ReceiverIdTxt>', s)
        if m:
            inter['receiver'] = m.group(1)
        m = re.match(r'Font=(.*\.xbf)', s)
        if m:
            inter['font'] = m.group(1)

print('=== Objects found ===')
for o in objects:
    font = o.get('font', '(default)')
    print(f"  {o['screen']:20s} {o['type']:8s} {o['id']:25s} {font}")

print()
print('=== Font Interactions ===')
for i in interactions:
    print(f"  {i['receiver']:30s} -> {i['font']}")
