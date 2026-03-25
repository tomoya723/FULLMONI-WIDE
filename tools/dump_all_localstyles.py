#!/usr/bin/env python3
"""Dump all localStyles from all widgets in eez-project."""
import json

proj = json.load(open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8'))
screen = proj['userPages'][0]['components'][0]

def walk(widgets):
    for w in widgets:
        ls = w.get('localStyles')
        if ls:
            print(json.dumps({'id': w.get('identifier', '?'), 'type': w.get('type', '?'), 'ls': ls}, indent=2))
            print('---')
        for child in w.get('children', []):
            walk([child])

walk(screen['children'])
