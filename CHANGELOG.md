# PyPRT ChangeLog

## v1.5.0 (2022-01-10)

### Added
* New PyEncoder option 'triangulate' to triangulate the geometry

### Changed
* Internal update to PRT 2.6
* Removed debug symbols from the native module in the released packages
* Development: expose the `setup.py --debug` option to switch between "Release" and "Release with Debug Info" for the native module
* Updated notebook, jupyterlab, pillow and babel versions in the environment files based on dependabot security analysis

## v1.4.0 (2021-08-24)

* Compatible CityEngine versions to create RPKs with: 2021.0 or earlier

### Added
* Added pre-built wheels and conda packages for Python 3.9

### Changed
* Internal update to PRT 2.4 (CityEngine 2021.0)
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
* Internal update to PRT 2.3
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
* Internal update to PRT 2.2 (adds support for CGA language features of CityEngine 2020.0)
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
