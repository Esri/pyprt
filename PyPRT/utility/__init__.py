import sys

try:
    __PYPRT_SETUP__
except NameError:
    __PYPRT_SETUP__ = False

if __PYPRT_SETUP__:
    sys.stderr.write('Running from source directory.\n')
else:
    from .utility import *