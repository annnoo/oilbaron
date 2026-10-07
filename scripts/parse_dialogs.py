#!/usr/bin/env python3
"""
parse_dialogs.py - Decompile Win16 NE dialog templates, menus, and accelerators
from game/OILBARON.EXE into standard Windows RC format (dialogs.rc).
"""

import os
import struct
import subprocess
import tempfile

def decode_dialog(data, dlg_name):
    # Win16 DLGTEMPLATE:
    # DWORD style
    # BYTE  cdit
    # short x, y, cx, cy
    style, cdit, x, y, cx, cy = struct.unpack_from('<IBhhhh', data, 0)
    pos = 13

    def read_str_or_id():
        nonlocal pos
        if pos >= len(data):
            return ''
        if data[pos] == 0xff:
            val = struct.unpack_from('<H', data, pos + 1)[0]
            pos += 3
            return str(val)
        else:
            end = data.find(b'\x00', pos)
            if end == -1:
                end = len(data)
            val = data[pos:end].decode('latin1', errors='replace')
            pos = end + 1
            return val

    menu = read_str_or_id()
    cls = read_str_or_id()
    caption = read_str_or_id()

    font_info = ''
    if style & 0x40:  # DS_SETFONT
        pt = struct.unpack_from('<H', data, pos)[0]
        pos += 2
        end = data.find(b'\x00', pos)
        if end == -1:
            end = len(data)
        fontname = data[pos:end].decode('latin1', errors='replace')
        pos = end + 1
        font_info = f'FONT {pt}, "{fontname}"'

    clean_name = f'"{dlg_name}"' if ' ' in dlg_name else dlg_name
    x_str = f'({x})' if x < 0 else str(x)
    y_str = f'({y})' if y < 0 else str(y)
    cx_str = f'({cx})' if cx < 0 else str(cx)
    cy_str = f'({cy})' if cy < 0 else str(cy)

    out = []
    out.append(f'// Dialog: {dlg_name}')
    out.append(f'{clean_name} DIALOG {x_str}, {y_str}, {cx_str}, {cy_str}')
    out.append(f'STYLE 0x{style:08X}L')
    if caption:
        out.append(f'CAPTION "{caption}"')
    if menu:
        out.append(f'MENU {menu}')
    if cls:
        out.append(f'CLASS "{cls}"')
    if font_info:
        out.append(font_info)
    out.append('BEGIN')

    classes = {
        0x80: '"Button"',
        0x81: '"Edit"',
        0x82: '"Static"',
        0x83: '"ListBox"',
        0x84: '"ScrollBar"',
        0x85: '"ComboBox"'
    }

    for _ in range(cdit):
        ix, iy, icx, icy, iid, istyle = struct.unpack_from('<hhhhHI', data, pos)
        pos += 14

        if data[pos] & 0x80:
            icls = classes.get(data[pos], f'"0x{data[pos]:02X}"')
            pos += 1
        else:
            end = data.find(b'\x00', pos)
            if end == -1:
                end = len(data)
            icls = f'"{data[pos:end].decode("latin1", errors="replace")}"'
            pos = end + 1

        if data[pos] == 0xff:
            res_id = struct.unpack_from('<H', data, pos + 1)[0]
            itxt = str(res_id)
            pos += 3
        else:
            end = data.find(b'\x00', pos)
            if end == -1:
                end = len(data)
            itxt = data[pos:end].decode('latin1', errors='replace')
            pos = end + 1

        extra_len = data[pos]
        pos += 1 + extra_len

        ctrl_id = -1 if iid == 65535 else iid
        safe_txt = itxt.replace('"', '""')
        out.append(f'    CONTROL "{safe_txt}", {ctrl_id}, {icls}, 0x{istyle:08X}L, {ix}, {iy}, {icx}, {icy}')

    out.append('END')
    return '\n'.join(out)


def decode_menu(data, menu_name):
    clean_name = f'"{menu_name}"' if ' ' in menu_name else menu_name
    out = [f'// Menu: {menu_name}', f'{clean_name} MENU', 'BEGIN']

    pos = 4  # skip 4-byte header

    def dump_level(pos, indent=4):
        while pos < len(data):
            flags = struct.unpack_from('<H', data, pos)[0]
            pos += 2
            is_popup = bool(flags & 0x10)
            is_end = bool(flags & 0x80)

            if is_popup:
                end = data.find(b'\x00', pos)
                if end == -1:
                    end = len(data)
                text = data[pos:end].decode('latin1', errors='replace')
                pos = end + 1
                safe_text = text.replace('"', '""')
                out.append(' ' * indent + f'POPUP "{safe_text}"')
                out.append(' ' * indent + 'BEGIN')
                pos = dump_level(pos, indent + 4)
                out.append(' ' * indent + 'END')
            else:
                mid = struct.unpack_from('<H', data, pos)[0]
                pos += 2
                end = data.find(b'\x00', pos)
                if end == -1:
                    end = len(data)
                text = data[pos:end].decode('latin1', errors='replace')
                pos = end + 1
                safe_text = text.replace('"', '""')
                if not safe_text and mid == 0:
                    out.append(' ' * indent + 'MENUITEM SEPARATOR')
                else:
                    out.append(' ' * indent + f'MENUITEM "{safe_text}", {mid}')

            if is_end:
                break
        return pos

    dump_level(pos)
    out.append('END')
    return '\n'.join(out)


def decode_accelerators(data, table_name):
    clean_name = f'"{table_name}"' if ' ' in table_name or table_name == 'ACCELERATORS' else table_name
    out = [f'// Accelerators: {table_name}', f'{clean_name} ACCELERATORS', 'BEGIN']

    pos = 0
    while pos < len(data):
        flags, key, aid = struct.unpack_from('<BHH', data, pos)
        pos += 5

        f_parts = []
        if flags & 0x01:
            f_parts.append('VIRTKEY')
        if flags & 0x02:
            f_parts.append('NOINVERT')
        if flags & 0x04:
            f_parts.append('SHIFT')
        if flags & 0x08:
            f_parts.append('CONTROL')
        if flags & 0x10:
            f_parts.append('ALT')

        flag_str = ', ' + ', '.join(f_parts) if f_parts else ''
        if 32 <= key < 127 and not (flags & 0x01):
            key_repr = f'"{chr(key)}"'
        else:
            key_repr = str(key)
        out.append(f'    {key_repr}, {aid}{flag_str}')

        if flags & 0x80:
            break

    out.append('END')
    return '\n'.join(out)


def main():
    exe_path = 'game/OILBARON.EXE'
    out_rc = 'decompile/resources/dialogs.rc'

    with tempfile.TemporaryDirectory() as tmpdir:
        # Extract dialogs
        subprocess.run(['wrestool', '-x', '-R', '-t', '5', exe_path, '-o', tmpdir], check=True)
        # Extract menus
        subprocess.run(['wrestool', '-x', '-R', '-t', '4', exe_path, '-o', tmpdir], check=True)
        # Extract accelerators
        subprocess.run(['wrestool', '-x', '-R', '-t', '9', exe_path, '-o', tmpdir], check=True)

        rc_sections = [
            '// Oil Baron (Win16 1993) - Extracted Resource Script',
            '// Generated by scripts/parse_dialogs.py',
            '#include <windows.h>',
            ''
        ]

        # Process dialogs
        rc_sections.append('// ===========================================================================')
        rc_sections.append('// DIALOG TEMPLATES')
        rc_sections.append('// ===========================================================================\n')

        dlg_files = sorted([f for f in os.listdir(tmpdir) if '_5_' in f])
        for fname in dlg_files:
            dlg_name = fname.split('_5_', 1)[1]
            with open(os.path.join(tmpdir, fname), 'rb') as f:
                data = f.read()
            rc_sections.append(decode_dialog(data, dlg_name))
            rc_sections.append('')

        # Process menus
        rc_sections.append('// ===========================================================================')
        rc_sections.append('// MENUS')
        rc_sections.append('// ===========================================================================\n')

        menu_files = sorted([f for f in os.listdir(tmpdir) if '_4_' in f])
        for fname in menu_files:
            menu_name = fname.split('_4_', 1)[1]
            with open(os.path.join(tmpdir, fname), 'rb') as f:
                data = f.read()
            rc_sections.append(decode_menu(data, menu_name))
            rc_sections.append('')

        # Process accelerators
        accel_files = sorted([f for f in os.listdir(tmpdir) if '_9_' in f])
        if accel_files:
            rc_sections.append('// ===========================================================================')
            rc_sections.append('// ACCELERATORS')
            rc_sections.append('// ===========================================================================\n')
            for fname in accel_files:
                accel_name = fname.split('_9_', 1)[1]
                with open(os.path.join(tmpdir, fname), 'rb') as f:
                    data = f.read()
                rc_sections.append(decode_accelerators(data, accel_name))
                rc_sections.append('')

        os.makedirs(os.path.dirname(out_rc), exist_ok=True)
        with open(out_rc, 'w') as f:
            f.write('\n'.join(rc_sections))

    print(f'Successfully generated {out_rc}')


if __name__ == '__main__':
    main()
