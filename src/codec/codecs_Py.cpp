/**
 * Esri CityEngine SDK Geometry Encoder for Python
 *
 * This example demonstrates the usage of the PRTX interface
 * to write custom encoders.
 *
 * See README.md in http://github.com/ArcGIS/esri-cityengine-sdk for build instructions.
 *
 * Written by Simon Haegler
 * Modified by Camille Lechot
 * Esri R&D Center Zurich, Switzerland
 *
 * Copyright 2012-2017 (c) Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "codecs_Py.h"

#include "encoder/PyEncoder.h"

#include "prtx/ExtensionManager.h"

#include <iostream>


extern "C" {


PYENC_EXPORTS_API void registerExtensionFactories(prtx::ExtensionManager* manager) {
	try {
		manager->addFactory(PyEncoderFactory::instance());
	} catch (std::exception& e) {
		std::cerr << __FUNCTION__ << " caught exception: " <<  e.what() << std::endl;
	} catch (...) {
		std::cerr << __FUNCTION__ << " caught unknown exception: " << std::endl;
	}
}


PYENC_EXPORTS_API void unregisterExtensionFactories(prtx::ExtensionManager* /*manager*/) {
}


PYENC_EXPORTS_API int getVersionMajor() {
	return PRT_VERSION_MAJOR;
}


PYENC_EXPORTS_API int getVersionMinor() {
	return PRT_VERSION_MINOR;
}


} // extern "C"
