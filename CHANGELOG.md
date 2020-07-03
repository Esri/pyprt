# PyPRT ChangeLog

## v1.1.0 (2020-07-YY)

### Added
* Added new API function `pyprt.inspect_rpk(...)` to query available CGA rule attributes.
* Added support for macOS 10.15 (Catalina).
* Added support for initial shapes with polygon holes.
* Added automatic detection of RuleFile and StartRule attributes in RPKs.
* The `GeneratedModel` class now provides access to CGA `print` and error output when used with the PyEncoder (new `get_cga_prints()` and `get_cga_errors()` functions).

### Changed
* Internal update to PRT 2.2 (adds support for CGA language features of CityEngine 2020.0).
* Reorganization and cleanup of C++ sources.

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