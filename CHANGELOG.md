# PyPRT ChangeLog

## v1.11.0 (2025-02-20)

### Added
* Added support for Rule Packages (RPK) created with CityEngine 2024.1
* Added support for Python 3.12, including pre-built wheels and conda packages.

### Changed
* Update to [PRT 3.2.10650](https://github.com/Esri/cityengine-sdk/blob/3.2.10650/changelog.md#cityengine-sdk-3210650-changelog)
* PyPRT and the underlying PRT library are now initialized and shutdown automatically (tied to the module "lifetime"). The following API functions are now considered deprecated and will emit warnings when called:
    * `initialize_prt`
    * `is_prt_initialized`
    * `shutdown_prt`
* Update to PyBind11 2.13.6

### Removed
* Removed support for Python 3.8 which reached end-of-life in October 2024.

## v1.10.0 (2024-07-17)

### Added
* Added support for Rule Packages (RPK) created with CityEngine 2024.0

### Changed
* Updated to [PRT 3.2.10211](https://github.com/Esri/cityengine-sdk/blob/3.2.10211/changelog.md#cityengine-sdk-3210211-changelog)
* Updated unit tests for PRT bug fix and adapted to behavior changes (see `readStringTable` and `convexify` in changelog).

## v1.9.0 (2024-07-17)

### Changed
* Updated to [PRT 3.2.9903](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-329903-changelog)
* Linux: fixed RPATH entries of various PRT libraries to make `auditwheel` work correctly.
* Raised build toolchains to MSVC 14.37 and RHEL 8, GCC 11 (as required by PRT).

## v1.8.0 (2024-03-05)

### Added
* Added support for Rule Packages (RPK) created with CityEngine 2023.1
* Added pre-built wheels and conda packages for Python 3.11
* Introduced `pyproject.toml` (calling setup.py directly has been deprecated by `setuptools`)

### Changed
* Updated to [PRT 3.1.9666](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-319666-changelog)
* Switched to the `build` package for wheel builds
* Reorganized the `src` directory to better support isolated builds
* Switched conda builds from `bdist_conda` to explicit conda recipe (`conda-recipe/meta.yaml`) and `conda build`
* Updated to PyBind11 2.11.1
* Raised minimum CMake version to 3.19
* Fixed source links in the API docs and improved the sidebar layout

### Removed
* Removed support for Python 3.7

## v1.7.0 (2023-06-12)

### Added
* Added support for Rule Packages (RPK) created with CityEngine 2023.0
* Added support for textured initial shapes (also added the `maxDirRecursionDepth` argument to
  the `InitialShape` class constructor to control searching for texture files)
* Added new API function `get_api_version` to get a list of the PRT (major, minor, build) components

### Changed
* Updated to PRT [3.0.8905](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-308905-changelog)
* Updated to PyBind11 2.10.4
* Turned the `prt_DIR` and `pybind11_DIR` into proper CMake cache variables

## v1.6.0 (2022-12-21)

### Added
* Added support for Rule Packages (RPK) created with CityEngine 2022.1
* Added pre-built wheels and conda packages for Python 3.10
* New CMake flag `pybind11_DIR` and env var `PYBIND11_DIR` for setup.py to let users specify a custom copy of PyBind11
* [New example (number 10)](https://github.com/esri/pyprt-examples) about updating Scene Layers used in Web Scenes

### Changed
* Update to [PRT 2.7.8538](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-278538-changelog)
* Rebuilt test RPKs with CityEngine 2022.1
* `pyprt_arcgis` module: added handling of polygons holes (inner rings) based on Shapely
* Cleaned and updated environment files
* Build Python with SSL support in Linux Docker images
* Improved conda environments pipeline

### Removed
* Removed Python 3.6 support
* Stop shipping unnecessary MSVC import libraries for PRT in the packages

## v1.5.0 (2022-01-10)

### Added
* New PyEncoder option 'triangulate' to triangulate the geometry

### Changed
* Internal update to [PRT 2.6](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-268300-changelog)
* Removed debug symbols from the native module in the released packages
* Development: expose the `setup.py --debug` option to switch between "Release" and "Release with Debug Info" for the native module
* Updated notebook, jupyterlab, pillow and babel versions in the environment files based on dependabot security analysis

## v1.4.0 (2021-08-24)

* Compatible CityEngine versions to create RPKs with: 2021.0 or earlier

### Added
* Added pre-built wheels and conda packages for Python 3.9

### Changed
* Internal update to [PRT 2.4](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-247316-changelog) (CityEngine 2021.0)
* Updated compiler requirements on Windows (MSVC 14.27) and Linux (GCC 9.3)
* Updated urllib3, pywin32 and pillow versions in the environment requirements-py3.*.txt files based on dependabot security analysis

### Removed
* Removed MacOS support as PRT 2.4 and later is not available anymore on that platform

## v1.3.0 (2021-05-12)

### Added
* New function `get_attributes` on `GeneratedModel`. Returns the CGA rule attributes used to generate the model
* Added support for CGA array attributes
* Added pre-built wheels and conda packages for Python 3.7 and 3.8

### Changed
* Updated the `arcgis_to_pyprt` function to work with the latest `arcgis` package (1.8)
* Switched from `pipenv` to `venv` to better support multiple Python versions

### Removed
* Removed previously deprecated API functions `inspect_rpk` and overload of `generate_model`
* Removed `tox` to simplify running tests in multiple Python versions

## v1.2.0 (2020-11-19)

### Added
* New `get_rpk_attributes_info(rule_package_path)` function to query CGA rule attributes and their annotations
* Holes and multi faces polygons support in the `arcgis_to_pyprt(...)` function
* PyPRT icon

### Changed
* Removal of the overload `generate_model(rule_attributes)` (use `generate_model(rule_attributes, rule_package_path, geometry_encoder, encoder_options)` instead)
* Deprecation of the `inspect_rpk(rule_package_path)` function (use `get_rpk_attributes_info(rule_package_path)` instead)
* Internal update to [PRT 2.3](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-236821-changelog)
* Updated compiler requirements on Windows (MSVC 14.23)

### Fixed
* Removed `streetWidth(a)` attribute from the rule package attributes dictionary (in `inspect_rpk(...)` and `get_rpk_attributes_info(...)`)

## v1.1.0 (2020-07-16)

### Added
* Added new API function `pyprt.inspect_rpk(...)` to query available CGA rule attributes
* Added support for macOS 10.15 (Catalina)
* Added support for initial shapes with polygon holes
* Added automatic detection of RuleFile and StartRule attributes in RPKs
* The `GeneratedModel` class now provides access to CGA `print` and error output when used with the PyEncoder (new `get_cga_prints()` and `get_cga_errors()` functions)

### Changed
* Internal update to [PRT 2.2](https://github.com/Esri/cityengine-sdk/blob/main/changelog.md#cityengine-sdk-226332-changelog) (adds support for CGA language features of CityEngine 2020.0)
* Reorganization and cleanup of C++ sources
* Moved PyPRT conda package to Esri organization

## v1.0.0 (2020-05-07)

### Added

* PyPRT conda package available on Anaconda Cloud
* API Documentation
* License file and copyright statements

### Changed

* README improvements
* setup.py metadata and PyPI project page improvements

## v1.0.0b1 (2020-02-20)

* First public release
* PyPRT wheels available on PyPI
* Published source code on GitHub
* Published [PyPRT examples](https://github.com/Esri/pyprt-examples) on GitHub
