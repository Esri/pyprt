# PyPRT - Python Bindings for CityEngine SDK

PyPRT provides a Python binding for PRT (Procedural RunTime) of CityEngine. This enables the execution of CityEngine CGA rules within Python. PyPRT allows to easily and efficiently generate 3D geometries, process them as Python data structures or export them to multiple 3D file formats.

![Procedural Generation of 3D Models](images/diagram.png)

## Table of Contents

* [Installation](#installation)
* [Minimal Example](#minimal-example)
* [Documentation](#documentation)
* [Development](#development)
* [Licensing Information](#licensing-information)

## Installation

Run `pip install pyprt` in your (virtual) Python environment or `conda install -c esri pyprt` in a Conda environment. Then use `import pyprt` in your scripts.

We provide wheels for Python 3.8 and 3.9 on Linux and Windows. Additionally, we also provide wheels for Python 3.7 on Windows. Conda packages are available for Python 3.7, 3.8 and 3.9 on Linux and Windows. For other Python versions please [build](#development) PyPRT yourself.


## Minimal Example

```python
# see example.py in repository root

import os
import pyprt

pyprt.initialize_prt()

# Define the input geometry
shape_geometry = pyprt.InitialShape([0, 0, 0, 0, 0, 100, 100, 0, 100, 100, 0, 0])

# Setup ModelGenerator instance for input geometry
model_generator = pyprt.ModelGenerator([shape_geometry])

# Setup generation parameters
repo_path = os.path.dirname(os.path.realpath(__file__))
rpk = os.path.join(repo_path, 'tests/data/extrusion_rule.rpk')
shape_attributes = {'shapeName': 'myShape', 'seed': 555}
encoder = 'com.esri.pyprt.PyEncoder'
encoder_options = {'emitReport': True, 'emitGeometry': True}

# Generate the model
generated_models = model_generator.generate_model([shape_attributes], rpk, encoder, encoder_options)

# Access the result
for model in generated_models:
    index = model.get_initial_shape_index()
    cga_report = model.get_report()
    vertices = model.get_vertices()
    print(f"Model {index} has vertices: {vertices} and reports {cga_report}")

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
  * Windows: MSVC 14.27 or later
  * Linux: GCC 9.3 or later (we build and test on RHEL7/CentOS7)
* Python (version >= 3.7)
  * Packages (latest version if not specified): wheel, arcgis, twine, sphinx, pkginfo, xmlrunner
* Optional: Conda (e.g. miniconda3)
* CMake (version >= 3.14)
* Ninja (or jom)

A note regarding the meaning of "open a shell" in the following sections: this implies that the shell also needs to have the correct C++ compiler activated:

* On Windows, use the shortcuts provided by Visual Studio or run `vcvarsall.bat` of the desired MSVC toolchain.
* On RHEL-based Linux, run e.g. `source /opt/rh/devtoolset-9/enable`.

_Note: on Windows, replace `bin` with `Scripts` in the following commands. Some commands also differ in their file extension between Linux and Windows (`.sh` vs `.bat` and similar). Please refer to the `venv` documentation for details: <https://docs.python.org/3.8/library/venv.html>_

### Build Python Wheel

1. Open a shell in the PyPRT git root.
1. First time only: set up a virtual Python environment with build dependencies for PyPRT. Adapt `python3.8` and the `envs` file to your desired OS/Python combination.
    1. Create the virtual environment: `python3.8 -m venv .venv`
    1. Get latest pip: `.venv/bin/python -m pip install --upgrade pip`
    1. Get latest wheel: `.venv/bin/python -m pip install --upgrade wheel`
    1. Install build dependencies for PyPRT: `.venv/bin/python -m pip install -r envs/centos7/wheel/requirements-py3.8.txt`
1. Run `.venv/bin/python setup.py bdist_wheel`. This will build the CMake project and Python packages. See [below](#c-debug-builds) for native debug mode.
1. The resulting wheel is written to the temporary `dist` folder.

### Build Conda Package

1. Install Miniconda or Anaconda.
1. Open a shell in the PyPRT git root and activate Miniconda (or Anaconda).
1. First time only: run `conda env create --prefix ./env --file envs/centos7/conda/environment-py3.8.yml` to create a conda environment with all the required Python packages (adapt `centos7` and `environment-py3.8.yml` to your desired OS/Python combination).
1. Run `activate ./env`.
1. Run `python setup.py bdist_conda`. This will build the CMake project and Python packages. See [below](#c-debug-builds) for native debug mode.
1. The resulting package is written to the `./env/conda-bld/{platform}` folder.

### Iterative Python Development

1. Open a shell in the PyPRT git root.
1. First time only: setup a virtual Python environment with build dependencies for PyPRT, see "Build Python Wheel" [above](#build-python-wheel).
1. Run `source .venv/bin/activate` (on Windows, `.venv\Scripts\activate.bat`) to activate the required Python packages.
1. Run `python setup.py clean --all` (to ensure we can properly track and cleanup the temporarily copied native extension).
1. Install PyPRT in current pip environment in dev mode by running `pip install -e .` (note the dot at the end). This will use CMake to build the native extension and copy them into the source directory. See [below](#c-debug-builds) for native debug mode.
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

#### C++ Debug Builds

By default, the native module is built in "release" mode (full optimization, no debug symbols). If the `--debug` argument is passed to `setup.py`, the CMake scripts will switch to "Release with Debug Info" mode, this will _disable optimization_ (`-O0`) to provide correct per-line debugging.

To create non-optimized wheels (or conda packages) with debug information:
`python setup.py build --debug bdist_wheel --skip-build` (or `bdist_conda`)

To enable debugging when iteratively working on the Python/C++ code use:
`pip install --install-option build --install-option --debug -e .`

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

### Build with Docker

Note: On Windows, Docker needs to be switched to "Windows Containers".

#### Build Wheels

1. Open a shell in the PyPRT git root
1. Create the base image for the desired build toolchain (adapt to your desired Python version):
   * Linux: `docker build --rm -f envs/centos7/base/Dockerfile -t pyprt-base:centos7-py3.8 --build-arg PY_VER=3.8 --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .`
   * Windows: `docker build --rm -f envs\windows\base\Dockerfile-py -t pyprt-base:windows-py3.8 --build-arg PY_VER=3.8 .`
1. Create the desired image for the build toolchain (adapt to your desired Python version):
   * Linux: `docker build --rm -f envs/centos7/wheel/Dockerfile -t pyprt:centos7-py3.8 --build-arg PY_VER=3.8 --build-arg BASE_TAG=centos7-py3.8 --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .`
   * Windows: `docker build --rm -f envs\windows\wheel\Dockerfile -t pyprt:windows-py3.8 --build-arg PY_VER=3.8 --build-arg BASE_TAG=windows-py3.8 .`
1. Run the build
   * Linux: `docker run --rm -v $(pwd):/tmp/pyprt/root -w /tmp/pyprt/root pyprt:centos7-py3.8 bash -c 'python setup.py bdist_wheel'`
   * Windows: `docker run --rm -v %cd%:C:\temp\pyprt\root -w C:\temp\pyprt\root pyprt:windows-py3.8 cmd /c "python setup.py bdist_wheel"`
1. The resulting wheel should appear in the `dist` directory.

#### Build Conda Packages

1. Open a shell in the PyPRT git root
1. Create the base image for the desired build toolchain (adapt `py3.8` to your desired Python version):
    * Linux: `docker build --rm -f envs/centos7/base/Dockerfile -t pyprt-base:centos7-py3.8 --build-arg PY_VER=3.8 --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .`
    * Windows: `docker build --rm -f envs\windows\base\Dockerfile-py -t pyprt-base:windows-py3.8 --build-arg PY_VER=3.8 .`
1. Create the desired image for the build toolchain (adapt `py3.8` to your desired Python version):
    * Linux: `docker build --rm -f envs/centos7/conda/Dockerfile -t pyprt:centos7-py3.8-conda --build-arg PY_VER=3.8 --build-arg BASE_TAG=centos7-py3.8 --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .`
    * Windows: `docker build --rm -f envs\windows\conda\Dockerfile -t pyprt:windows-py3.8-conda --build-arg PY_VER=3.8 --build-arg BASE_TAG=windows-py3.8 .`
1. Run the build
    * Linux: `docker run --rm -v $(pwd):/tmp/pyprt/root -w /tmp/pyprt/root pyprt:centos7-py3.8-conda bash -c 'python setup.py bdist_conda && cp -r /tmp/pyprt/pyprt-conda-env/conda-bld/linux-64/pyprt*.tar.bz2 /tmp/pyprt/root'`
    * Windows: `docker run --rm -v %cd%:C:\temp\pyprt\root -w C:\temp\pyprt\root pyprt:windows-py3.8-conda cmd /c "python setup.py bdist_conda && copy C:\temp\conda\envs\pyprt\conda-bld\win-64\pyprt-*.tar.bz2 C:\temp\pyprt\root"`
1. The resulting conda package will be located in the current directly (PyPRT git repo root).

## Licensing Information

PyPRT is free for personal, educational, and non-commercial use. Commercial use requires at least one commercial license of the latest CityEngine version installed in the organization. Redistribution or web service offerings are not allowed unless expressly permitted.

PyPRT is under the same license as the included [CityEngine SDK](https://github.com/esri/cityengine-sdk#licensing). An exception is the PyPRT source code (without CityEngine SDK, binaries, or object code), which is licensed under the Apache License, Version 2.0 (the “License”); you may not use this work except in compliance with the License. You may obtain a copy of the License at https://www.apache.org/licenses/LICENSE-2.0

All content in the "Examples" directory/section is licensed under the APACHE 2.0 license as well.

For questions or enquiries, please contact the Esri CityEngine team (cityengine-info@esri.com).

[Back to top](#table-of-contents)
