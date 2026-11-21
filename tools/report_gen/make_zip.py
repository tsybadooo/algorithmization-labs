import os
import zipfile

root = 'D:/Downloads/alg/vykonannya/kpiz'
out = 'D:/Downloads/alg/Звіти/КПІЗ_вихідні_коди_Цибульський_АВ-11.zip'
skip_ext = {'.exe', '.o', '.gcda', '.gcno', '.dat'}
n = 0
with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
    for d, dirs, files in os.walk(root):
        dirs[:] = [x for x in dirs if x != 'build']
        for f in files:
            if os.path.splitext(f)[1] in skip_ext or f in ('gdb_input.txt', 'gdb_cmds.txt'):
                continue
            p = os.path.join(d, f)
            z.write(p, 'kpiz/' + os.path.relpath(p, root).replace(os.sep, '/'))
            n += 1
print('zip files:', n, os.path.getsize(out), 'bytes')
