# PyPRT - Python Bindings for CityEngine SDK

## Installation

Simply run `pip install pyprt` in your desired Python environment, then use `import pyprt` in your scripts. See here TODO for API reference.

TODO: add doc for `conda`

## Development

### Project Overview
TODO: explain native directory (src) and Python dir (pyprt)

### Requirements
* C++ Compiler (C++ 17)
  * Windows: MSVC 14.16 or later
  * Linux: GCC 8 or later
* Python (version >= 3.6) with pip and pipenv
* CMake (version >= 3.14)
* Ninja (or jom)

### Build Python Wheel

1. In the PyPRT git root, open a shell and activate correct C++ compiler (`vcvarsall.bat` on Windows or `source /opt/rh/devtoolset-8/enable` on RHEL-based Linux).
1. First time only: run `pipenv install` to get all required Python packages.
1. Run `pypenv run python setup.py bdist_wheel`. This will build the CMake project and Python packages.
1. The resulting wheel is written to the temporary `dist` folder.

### Iterative Python Development

1. In the PyPRT git root, open a shell and activate correct C++ compiler (`vcvarsall.bat` on Windows or `source /opt/rh/devtoolset-8/enable` on RHEL-based Linux).
1. First time only: run `pipenv install` to get all required Python packages.
1. Run `pipenv shell` to activate the required Python packages.
1. Install PyPRT in current pip environment in dev mode by running `pip install -e .` (note the dot at the end). This will use CMake to build the native extension and copy them into the source directory.
1. Now you can iterate on the python part of PyPRT...
1. To leave development mode and cleanup your git workspace run these commands:
   1. `pip uninstall pyprt` (this will remove the "symlinked" package for the current pip env)
   1. `python setup.py clean` (this will remove the copied native extension) XXX TODO XXX
1. Run `exit` to leave the pipenv shell.

### Iterative C++ Development

The `src` directory contains a standard CMake project with the PyPRT native extension.

The workflow is the same as iterating on the Python files but needs additional setup of CMAKE_INSTALL_PREFIX to point into the `pyprt/pyprt` directory.
This will allow you to run `ninja install` after changes on the C++ files and update the "live" PyPRT package in the `pyprt` subdir.
The `setup.py clean` call mentioned above will also clean out the native extension from the `pyprt` source directory.

### Running Unit Tests

1. In the PyPRT git root, open a shell and activate correct C++ compiler (`vcvarsall.bat` on Windows or `source /opt/rh/devtoolset-8/enable` on RHEL-based Linux).
1. First time only: run `pipenv install` to get all required Python packages.
1. Run `pipenv run tox`

## LICENSING

TODO