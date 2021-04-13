# PyPRT - Python Bindings for CityEngine SDK

PyPRT is a Python binding for PRT (CityEngine Procedural Runtime). It enables the execution of CityEngine CGA rules within Python. PyPRT allows to easily and efficiently generate 3D geometries, process them as Python data structures or export them to multiple 3D file formats.

![Procedural Generation of 3D Models](images/diagram.png)

## Table of Contents

* [Installation](#installation)
* [Minimal Usage](#minimal-usage)
* [Documentation](#documentation)
* [Development](#development)
* [License](#license)

## Installation

For Python 3.6 (64bit), run `pip install pyprt` in your (virtual) Python environment or `conda install -c esri pyprt` in a Conda environment. Then use `import pyprt` in your scripts. For other Python versions, please [build](#development) PyPRT yourself at the moment.

## Minimal Usage

```python
import os
import pyprt

# PRT Initialization
pyprt.initialize_prt()

# Initial Shape
shape_geometry = pyprt.InitialShape([0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])

# ModelGenerator Instance
m = pyprt.ModelGenerator([shape_geometry])

# Model Generation Arguments Setup
rpk = os.path.join(os.getcwd(), 'extrusion_rule.rpk')
shape_attributes = {'shapeName': 'myShape', 'seed': 555}
encoder = 'com.esri.pyprt.PyEncoder'
encoder_options = {'emitReport': True, 'emitGeometry': True}

# PRT Generation
generated_models = m.generate_model([shape_attributes], rpk, encoder, encoder_options)

# Info Collection
for model in generated_models:
    id = model.get_initial_shape_index()
    cga_report = model.get_report()
    vertices_coordinates = model.get_vertices()

# PRT Shutdown
pyprt.shutdown_prt()
```

## Documentation

* [API Reference](https://esri.github.io/pyprt/apidoc/index.html)
* [Changelog](CHANGELOG.md)
* [Authoring of Rule Packages](https://doc.arcgis.com/en/cityengine/latest/help/help-rule-package.htm#ESRI_SECTION1_F9D4CCCE0EC74E5FB646A8BD141A38F9)
* [Examples](https://github.com/Esri/pyprt-examples)
* [CityEngine SDK API Reference](https://esri.github.io/cityengine-sdk/html/index.html)

## Development

### Project Overview

The project is composed of two parts: the C++ native directory (`src`) and Python-only directory (`pyprt`). The C++ part contains a standard CMake project with PyPRT native extension. The Python bindings are done using [pybind11](https://pybind11.readthedocs.io/en/stable/intro.html). The `pyprt` directory contains Python helper functions.

### Requirements

* C++ Compiler (C++ 17)
  * Windows: MSVC 14.23 or later
  * Linux: GCC 8 or later (we build and test on RHEL7/CentOS7)
  * macOS (Catalina or later): Xcode 11
* Python (version >= 3.6)
  * wheel, arcgis, twine, sphinx, pkginfo, xmlrunner
* CMake (version >= 3.14)
* Ninja (or jom)

A note regarding "open a shell" in the following sections: this implies that the shell also needs to have the correct C++ compiler activated:

* On Windows, use the shortcuts provided by Visual Studio or run `vcvarsall.bat` of the desired MSVC toolchain.
* On RHEL-based Linux, run e.g. `source /opt/rh/devtoolset-8/enable`.
* On macOS, there is usually no action required (the current Xcode command line tools are activated by default).

_Note: on Windows, replace `bin` with `Scripts` in the following commands. Some commands also differ in their file extension between Linux and Windows (`.sh` vs `.bat` and similar). Please refer to the `venv` documentation for details: <https://docs.python.org/3.6/library/venv.html>_

### Build Python Wheel

1. Open a shell in the PyPRT git root.
1. First time only: setup a virtual Python environment with build dependencies for PyPRT. Adapt `python3.6` and `centos7/py36` to your desired OS/Python combination.
    1. Create the virtual environment: `python3.6 -m venv .venv`
    1. Get latest pip: `.venv/bin/python -m pip install --upgrade pip`
    1. Get latest wheel: `.venv/bin/python -m pip install --upgrade wheel`
    1. Install build dependencies for PyPRT: `.venv/bin/python -m pip install -r envs/centos7/py36/requirements.txt`
1. Run `.venv/bin/python setup.py bdist_wheel`. This will build the CMake project and Python packages.
1. The resulting wheel is written to the temporary `dist` folder.

### Build Conda Package

1. Install Miniconda or Anaconda.
1. Open a shell in the PyPRT git root and activate Miniconda (or Anaconda).
1. First time only: run `conda env create --prefix ./env --file environment.yml` to create a conda environment with all the required Python packages.
1. Run `activate ./env`.
1. Run `python setup.py bdist_conda`. This will build the CMake project and Python packages.
1. The resulting package is written to the `./env/conda-bld/{platform}` folder.

### Iterative Python Development

1. Open a shell in the PyPRT git root.
1. First time only: setup a virtual Python environment with build dependencies for PyPRT, see "Build Python Wheel" [above](#build-python-wheel).
1. Run `source .venv/bin/activate` (on Windows, `.venv\Scripts\activate.bat`) to activate the required Python packages.
1. Run `python setup.py clean --all` (to ensure we can properly track and cleanup the temporarily copied native extension).
1. Install PyPRT in current pip environment in dev mode by running `pip install -e .` (note the dot at the end). This will use CMake to build the native extension and copy them into the source directory.
1. Now you can iterate on the Python part of PyPRT...
1. To leave development mode and cleanup your git workspace, run these commands:
   1. `pip uninstall pyprt` (this will remove the "symlinked" package for the current pip env)
   1. `python setup.py clean` (this will remove the copied native extension)
1. Run `deactivate` to leave the venv shell.

### Iterative C++ Development

The `src` directory contains a standard CMake project with the PyPRT native extension.

The workflow is the same as iterating on the Python files but needs additional setup of CMAKE_INSTALL_PREFIX to point into the `pyprt/pyprt` directory.
This will allow you to run `ninja install` after changes on the C++ files and update the "live" PyPRT package in the `pyprt` subdir.
The `setup.py clean` call mentioned above will also clean out the native extension from the `pyprt` source directory.

### Running Unit Tests

Quick way to build and run all tests in a separate virtual environment:

1. Open a shell in the PyPRT git root.
1. Run helper script: `python build_and_run_tests.py`

Detailed steps to run tests for development (basically what the `build_and_run_tests.py` script does):

1. Open a shell in the PyPRT git root.
1. First time only: setup a virtual Python environment with build dependencies for PyPRT, see "Build Python Wheel" [above](#build-python-wheel).
1. Build and install PyPRT into local env: `.venv/bin/python setup.py install`
1. Run the tests: `.venv/bin/python tests/run_tests.py`
1. Uninstall PyPRT from local env: `.venv/bin/python -m pip uninstall pyprt`
1. Cleanup intermediate build files: `.venv/bin/python setup.py clean --all`

### Build the API documentation

1. Install PyPRT in development mode as described [above](#iterative-python-development).
1. Run `python setup.py build_doc`, this will output the html files in the `build/sphinx` directory.
1. Leave development mode also as described [above](#iterative-python-development).

### Docker

1. Open a shell in the PyPRT git root
1. Create the desired image for the build toolchain (adapt `centos7/py36` with your desired OS/Python combination): `docker build --rm -f envs/centos7/py36/Dockerfile -t pyprt:centos7-py36 .`
1. Run the build
   * Linux: `docker run --rm -v $(pwd):/tmp/pyprt/root -w /tmp/pyprt/root pyprt:centos7-py36 python setup.py bdist_wheel`
   * Windows: `docker run --rm -v %cd%:C:\temp\pyprt\root -w C:\temp\pyprt\root pyprt:windows-py36 python setup.py bdist_wheel`

## License

PyPRT is under the same license as the included [CityEngine SDK](https://github.com/Esri/cityengine-sdk#licensing).

An exception is the PyPRT source code (without CityEngine SDK, binaries, or object code), which is licensed under the Apache License, Version 2.0 (the “License”); you may not use this work except in compliance with the License. You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>.

[Back to top](#table-of-contents)
