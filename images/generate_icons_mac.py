import os
import subprocess

# Create the directory if need be.
logoDir = 'logo.iconset'

try:
    os.makedirs('logo.iconset')
except(OSError):
    pass

sizes = [16, 32, 128, 256, 512]

def callInkscape(fileName, size):
    # Inkscape must be in the system path.
    # E.g. >> inkscape --export-png icon_512x512.png -w 512 -h 512 logo.svg
    subprocess.call(['inkscape', '--export-png', fileName,
        '-w', str(size), '-h', str(size), 'logo.svg'])

# Use inkscape to generate multiple resolutions of icon.
for s in sizes:
    callInkscape('%s/icon_%dx%d.png' % (logoDir, s, s), s)
    callInkscape('%s/icon_%dx%d@2x.png' % (logoDir, s, s), 2*s)

# Make an icns file out of the iconset
subprocess.call(['iconutil', '-c', 'icns', logoDir])