import os

try:
    __PYPRT_SETUP__
except NameError:
    __PYPRT_SETUP__ = False

if __PYPRT_SETUP__:
    sys.stderr.write('Running from source directory.\n')
else:
    from .bin.pyprt import *
    PYPRT_SDK_PATH = os.path.join(os.path.dirname(__file__), 'bin')
