#!/usr/bin/env python3
"""Inspect HeadUpDaisy TTF to understand available sizes."""
from fontTools.ttLib import TTFont

for path in ['Firmware/eez/x14y24pxHeadUpDaisy.ttf', 'Firmware/eez/x8y12pxDenkiChip.ttf']:
    print(f'=== {path} ===')
    font = TTFont(path)
    print(f'  Font name: {font["name"].getDebugName(4)}')
    print(f'  Units per em: {font["head"].unitsPerEm}')
    
    has_bitmaps = any(t in font for t in ['EBDT', 'EBLC', 'CBDT', 'CBLC'])
    print(f'  Embedded bitmaps: {has_bitmaps}')
    
    os2 = font['OS/2']
    print(f'  Ascent: {os2.sTypoAscender}, Descent: {os2.sTypoDescender}')
    print(f'  Win ascent: {os2.usWinAscent}, Win descent: {os2.usWinDescent}')
    
    if 'gasp' in font:
        print(f'  gasp ranges: {font["gasp"].gaspRange}')
    
    # Check for bitmap strikes
    if 'EBLC' in font:
        for strike in font['EBLC'].strikes:
            print(f'  Bitmap strike: ppem={strike.bitmapSizeTable.ppemX}x{strike.bitmapSizeTable.ppemY}')
    
    print(f'  Tables: {sorted(font.keys())}')
    print()
