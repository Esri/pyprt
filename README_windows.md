PyPRT - CityEngine SDK Python Bindings for PRT
------------------------------------------


PREREQUISITES
-------------

Before you start working with this example, please make sure you follow
the main installation instructions for the CityEngine SDK in the
README.md file at the root of the CityEngine SDK example [repository](https://github.com/Esri/esri-cityengine-sdk). This will 
explain how to get the sdk binaries and example data.


SOFTWARE REQUIREMENTS
---------------------

- see "General Software Requirements" (Windows)
- Make sure you use the **exact** compiler for PRT extensions
- Python version: 3.6


INSTALLATION INSTRUCTIONS
---------------------

1. Download the SDK binary archive from the release page
1. Download the example data archive from the [release page](https://github.com/Esri/esri-cityengine-sdk)
1. Unzip the archives into the cloned repository into a "prt" and "data" directory
1. Download and unzip [pybind11](https://github.com/pybind/pybind11/tree/stable)
1. Download and unzip this repository and place it following the layout below.

Make modifications for the final directory layout to look like this:
```
/esri-cityengine-sdk/
    examples/
		py4prt/
			src/
				client/...
				codec/...
				pybind11/...
				CMakeLists.txt
				dependencies.cmake
    data/...
    prt/
        bin/...
        cmake/...
        doc/...
        include/...
        lib/...
```


BUILD INSTRUCTIONS
------------------

1. Open a `VS2017 x64 Native Tools Command Prompt`
1. Change into the example directory: `cd <your path to>\esri-cityengine-sdk\examples\py4prt`
1. Create a build directory: `mkdir build`
1. Change into the build directory: `cd build`
1. Run cmake: `cmake -G "NMake Makefiles" -CMAKE_BUILD_TYPE=RelWithDebSym ..\src`
1. Compile: `nmake install`
1. The build result will appear in the `install` directory in parallel to the `build` directory.


USING PY4PRT
-------------

1. Open a cmd shell and `cd` into the above `install\bin` directory.
1. Run the `test.py` scrit.
1. Expected output:
```
	Test Function: it should print 407.
	407

	Size of the matrix containing all the model vertices:
	(8, 6)

	Report of the generated model:
	Building Height.0 , 11.216290
	Min Height.0 , 10.000000
	Max Height.0 , 30.000000
	Id , 0.000000
	Parcel Area.0 , 4848.810925
```


LICENSING
---------

Copyright (c) 2018 Esri

You may not use the content of this repository except in compliance with the following Licenses:
  1. All content of all directories **except "examples"** is licensed under the CityEngine EULA, see [license/EULA.pdf](license/EULA.pdf).
  2. All content in the "examples" directory tree is licensed under the APACHE 2.0 license. You may obtain a copy of this license at http://www.apache.org/licenses/LICENSE-2.0. A copy of the license is also available in the repository at [license/APACHE-LICENSE-2.0.txt](license/APACHE-LICENSE-2.0.txt).
