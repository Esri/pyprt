PyPRT - CityEngine SDK Python Bindings for PRT
------------------------------------------


SOFTWARE REQUIREMENTS
---------------------

- see "General Software Requirements" (Windows)
- Make sure you use the **exact** compiler for PRT extensions
- Python (version >= 3.4)
- CMake (version >= 3.12)


INSTALLATION INSTRUCTIONS
---------------------

1. Clone this repository
2. Download the SDK binary archive from the release page as explained here: [repository](https://github.com/Esri/esri-cityengine-sdk)
3. Unzip the archives into the cloned repository into a "prt" directory

Make modifications for the final directory layout to look like this:
```
/py4prt/
    src/
      client/...
      codec/...
      CMakeLists.txt
      dependencies.cmake
    caseStudy/...
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
2. Change into the example directory: `cd <your path to>\py4prt`
3. Create a build directory: `mkdir build`
4. Change into the build directory: `cd build`
5. Run cmake: `cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPYTHON_EXECUTABLE=path\to\python ..\src`
6. Compile: `nmake install`
7. The build result will appear in the `install` directory in parallel to the `build` directory.

VISUAL STUDIO INSTRUCTIONS
------------------

1. Open a command prompt
2. Change into the example directory: `cd <your path to>\py4prt`
3. Create a build directory: `mkdir build`
4. Change into the build directory: `cd build`
5. Run cmake: `cmake -G "Visual Studio 15 2017 Win64" -DPYTHON_EXECUTABLE=path\to\python -DCMAKE_BUILD_TYPE="RelWithDebInfo" ..\src`
6. Open the solution file `build/pyprt.sln` and build/install from Visual Studio
7. The build result will appear in the `install` directory in parallel to the `build` directory.



USING PYPRT
-------------

1. Run the `caseStudy/test.py` script (from py4prt folder)


LICENSING
---------

Copyright (c) 2018 Esri

You may not use the content of this repository except in compliance with the following Licenses:
  1. All content of all directories **except "examples"** is licensed under the CityEngine EULA, see [license/EULA.pdf](license/EULA.pdf).
  2. All content in the "examples" directory tree is licensed under the APACHE 2.0 license. You may obtain a copy of this license at http://www.apache.org/licenses/LICENSE-2.0. A copy of the license is also available in the repository at [license/APACHE-LICENSE-2.0.txt](license/APACHE-LICENSE-2.0.txt).
