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
#include <numeric>
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

class InitialShape {
public:
	InitialShape(const std::vector<double>& vert);
	InitialShape(const std::vector<double>& vert, const std::vector<uint32_t>& ind,
	             const std::vector<uint32_t>& faceCnt);
	InitialShape(const std::string& path);
	~InitialShape() {}

	const double* getVertices() const {
		return mVertices.data();
	}
	size_t getVertexCount() const {
		return mVertices.size();
	}
	const uint32_t* getIndices() const {
		return mIndices.data();
	}
	size_t getIndexCount() const {
		return mIndices.size();
	}
	const uint32_t* getFaceCounts() const {
		return mFaceCounts.data();
	}
	size_t getFaceCountsCount() const {
		return mFaceCounts.size();
	}
	const std::string& getPath() const {
		return mPath;
	}
	bool getPathFlag() const {
		return mPathFlag;
	}

protected:
	const std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaceCounts;
	const std::string mPath;
	const bool mPathFlag;
};

class GeneratedModel {
public:
	GeneratedModel(const size_t& initialShapeIdx, const std::vector<double>& vert, const std::vector<uint32_t>& indices,
	               const std::vector<uint32_t>& face, const py::dict& rep);
	GeneratedModel() {}
	~GeneratedModel() {}

	size_t getInitialShapeIndex() const {
		return mInitialShapeIndex;
	}
	const std::vector<double>& getVertices() const {
		return mVertices;
	}
	const std::vector<uint32_t>& getIndices() const {
		return mIndices;
	}
	const std::vector<uint32_t>& getFaces() const {
		return mFaces;
	}
	const py::dict& getReport() const {
		return mReport;
	}

private:
	size_t mInitialShapeIndex;
	std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaces;
	py::dict mReport;
};

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
