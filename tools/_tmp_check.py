import json
with open('Firmware/eez/fullmoni.eez-project', 'r', encoding='utf-8') as f:
    proj = json.load(f)

print('Top-level keys:', list(proj.keys()))
for k, v in proj.items():
    if isinstance(v, list):
        print(f'  {k}: list[{len(v)}]')
    elif isinstance(v, dict):
        print(f'  {k}: dict keys={list(v.keys())[:10]}')
    else:
        print(f'  {k}: {type(v).__name__} = {str(v)[:80]}')
