import os
import sys

# current test workflow expects the developer to run 'setup.py bdist' first,
# so we append to the python module path
pyprt_build_dir = 'lib.win-amd64-3.6' if sys.platform.startswith('win32') else 'lib.linux-x86_64-3.6'
SDK_PATH = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'build', pyprt_build_dir)
sys.path.append(SDK_PATH)

import pyprt

if __name__ == '__main__':
    print()
    print('If arcgis package is not installed, the script should throw an ImportError.')
    print('If arcgis package is installed, the pyprt_arcgis module details should be displayed.')
    print()

    from pyprt.pyprt_arcgis import arcgis_to_pyprt
    help(arcgis_to_pyprt)
