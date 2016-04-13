from tempfile import mkstemp
from shutil import move
from os import remove, close

import os
import glob
import re


def insert_export():
    headers = glob.glob('old/*.h')

    for header in headers:
        fh, tmp = mkstemp()
        with open(tmp, 'w') as new_file:
            with open(header, 'r') as old_file:
                for line in old_file:
                    newline, n = re.subn(r'\A(class)(\s*\w*\s*.*[^;]\n)', r'\1 Q_DECL_EXPORT\2', line)
                    new_file.write(newline)
                    if n > 0:
                        print(newline)

        close(fh)
        # move(header, 'old/' + header)
        move(tmp, os.path.split(header)[1])

if __name__ == '__main__':
    insert_export()
