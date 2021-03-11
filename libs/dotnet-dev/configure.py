import os
import sys

line = "using python : {PY_VER} : {PREFIX}\python.exe : {PREFIX}\include : {PREFIX}\libs ;\n"
line = line.format(**os.environ).replace("\\", "\\\\")

sys.stdout.write(line)
