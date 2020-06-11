/**
 * CityEngine SDK Geometry Encoder for Python
 *
 * Copyright (c) 2012-2020 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * A copy of the license is available in the repository's LICENSE file.
 */

#pragma once

#include "InitialShape.h"
#include "GeneratedModel.h"
#include "PyCallbacks.h"
#include "logging.h"
#include "utils.h"

#include "prt/API.h"
#include "prt/ContentType.h"
#include "prt/LogLevel.h"
#include "prt/prt.h"

#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#ifdef _WIN32
#	include <direct.h>
#endif

namespace py = pybind11;

// cstr must have space for cstrSize characters
// cstr will be null-terminated and the actually needed size is placed in
// cstrSize
inline void copyToCStr(const std::string& str, char* cstr, size_t& cstrSize) {
	if (cstrSize > 0) {
		strncpy(cstr, str.c_str(), cstrSize);
		cstr[cstrSize - 1] = 0x0; // enforce null-termination
	}
	cstrSize = str.length() + 1; // returns the actually needed size including terminating null
}


namespace {

class ModelGenerator {
public:
	ModelGenerator(const std::vector<InitialShape>& myGeo);
	~ModelGenerator() {}

	std::vector<GeneratedModel> generateModel(const std::vector<py::dict>& shapeAttributes,
	                                          const std::filesystem::path& rulePackagePath,
	                                          const std::wstring& geometryEncoderName,
	                                          const py::dict& geometryEcoderOptions);
	std::vector<GeneratedModel> generateAnotherModel(const std::vector<py::dict>& shapeAttributes);

private:
	pcu::ResolveMapPtr mResolveMap;
	pcu::CachePtr mCache;

	pcu::AttributeMapBuilderPtr mEncoderBuilder;
	std::vector<pcu::AttributeMapPtr> mEncodersOptionsPtr;
	std::vector<std::wstring> mEncodersNames;
	std::vector<pcu::InitialShapeBuilderPtr> mInitialShapesBuilders;

	std::wstring mRuleFile = L"";
	std::wstring mStartRule = L"";
	int32_t mSeed = 0;
	std::wstring mShapeName = L"InitialShape";

	bool mValid = true;

	void setAndCreateInitialShape(const std::vector<py::dict>& shapeAttr,
	                              std::vector<const prt::InitialShape*>& initShapes,
	                              std::vector<pcu::InitialShapePtr>& initShapesPtrs,
	                              std::vector<pcu::AttributeMapPtr>& convertShapeAttr);
	void initializeEncoderData(const std::wstring& encName, const py::dict& encOpt);
	void getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc,
	                               std::vector<const prt::AttributeMap*>& allEncOpt);
};

} // namespace
