/**
 * Esri CityEngine SDK Geometry Encoder for Python
 *
 * Copyright 2014-2019 Esri R&D Zurich and VRBN
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
 */


#include "utils.h"
#include "logging.h"
#include "PyCallbacks.h"

#include "prt/prt.h"
#include "prt/API.h"
#include "prt/ContentType.h"
#include "prt/LogLevel.h"

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

#include <string>
#include <vector>
#include <iterator>
#include <functional>
#include <array>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <numeric>
#include <filesystem>
#ifdef _WIN32
#	include <direct.h>
#endif

namespace py = pybind11;

extern pcu::Path executablePath;


// cstr must have space for cstrSize characters
// cstr will be null-terminated and the actually needed size is placed in cstrSize
void copyToCStr(const std::string& str, char* cstr, size_t& cstrSize) {
	if (cstrSize > 0) {
		strncpy(cstr, str.c_str(), cstrSize);
		cstr[cstrSize - 1] = 0x0; // enforce null-termination
	}
	cstrSize = str.length() + 1; // returns the actually needed size including terminating null
}

// convert a vector of vertices coordinates into a vector of vectors with the x, y,
// z coordinates of the vertices
std::vector<std::vector<double>> convertVerticesIntoPythonStyle(const std::vector<double>& verticesList) {
    std::vector<std::vector<double>> vertices;
    vertices.resize(verticesList.size() / 3);

    for (size_t i = 0; i < verticesList.size() / 3; i++)
        vertices[i] = { verticesList[i*3], verticesList[i*3+1], verticesList[i*3+2] };

    return vertices;
}


/**
  * custom console logger to redirect PRT log events into the python output
  */
class PythonLogHandler : public prt::LogHandler {
public:
	PythonLogHandler() = default;
	virtual ~PythonLogHandler() = default;

	virtual void handleLogEvent(const wchar_t* msg, prt::LogLevel /*level*/) {
		pybind11::print(L"[PRT]", msg);
	}

	virtual const prt::LogLevel* getLevels(size_t* count) {
		*count = prt::LogHandler::ALL_COUNT;
		return prt::LogHandler::ALL;
	}

	virtual void getFormat(bool* dateTime, bool* level) {
		*dateTime = true;
		*level = true;
	}

	virtual char* toXML(char* result, size_t* resultSize, prt::Status* stat = 0) const {
		std::ostringstream out;
		out << *this;
		copyToCStr(out.str(), result, *resultSize);
		if (stat) *stat = prt::STATUS_OK;
		return result;
	}

	friend std::ostream& operator<<(std::ostream& stream, const PythonLogHandler&) {
		stream << "<PythonLogHandler />";
		return stream;
	}

private:
	const prt::LogLevel*    mLevels;
	size_t                  mCount;
};


/**
  * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
  */
struct PRTContext {
	PRTContext(prt::LogLevel minimalLogLevel, std::string const & sdkPath) {
		executablePath = sdkPath.empty() ? pcu::getExecutablePath() : sdkPath;
		const pcu::Path installPath = executablePath.getParent();

		prt::addLogHandler(&mLogHandler);

		// setup paths for plugins, assume standard SDK layout as per README.md
		const pcu::Path extPath = installPath / "lib";

		// initialize PRT with the path to its extension libraries, the default log level
		const std::wstring wExtPath = extPath.native_wstring();
		const std::array<const wchar_t*, 1> extPaths = { wExtPath.c_str() };
		mPRTHandle.reset(prt::init(extPaths.data(), extPaths.size(), minimalLogLevel));
	}

	~PRTContext() {
		// shutdown PRT
		mPRTHandle.reset();

		// remove loggers
		prt::removeLogHandler(&mLogHandler);
	}

	explicit operator bool() const {
		return (bool)mPRTHandle;
	}

	PythonLogHandler    mLogHandler;
	pcu::ObjectPtr      mPRTHandle;
};


class Geometry {
public:
	Geometry(const std::vector<double>& vert);
	Geometry() { }
	~Geometry() { }

	void updateGeometry(const std::vector<double>& vert, const size_t& vertCnt, const std::vector<uint32_t>& ind, const size_t& indCnt, const std::vector<uint32_t>& faceCnt, const size_t& faceCntCnt);
	const double* getVertices() const { return mVertices.data(); }
	size_t getVertexCount() const { return mVertexCount; }
	const uint32_t* getIndices() const { return mIndices.data(); }
	size_t getIndexCount() const { return mIndexCount; }
	const uint32_t* getFaceCounts() const { return mFaceCounts.data(); }
	size_t getFaceCountsCount() const { return mFaceCountsCount; }

protected:
	std::vector<double>     mVertices;
	size_t                  mVertexCount;
	std::vector<uint32_t>   mIndices;
	size_t                  mIndexCount;
	std::vector<uint32_t>   mFaceCounts;
	size_t                  mFaceCountsCount;
};


class GeneratedGeometry {
public:
	GeneratedGeometry(const size_t& initialShapeIdx, const std::vector<std::vector<double>>& vert, const std::vector<std::vector<uint32_t>>& face, const FloatMap& floatRep, const StringMap& stringRep, const BoolMap& boolRep);
	GeneratedGeometry() { }
	~GeneratedGeometry() { }

	size_t getInitialShapeIndex() const { return mInitialShapeIndex; }
	const std::vector<std::vector<double>>& getVertices() const { return mVertices; }
	const std::vector<std::vector<uint32_t>>& getFaces() const { return mFaces; }
	const FloatMap& getFloatReport() const { return mFloatReport; }
	const StringMap& getStringReport() const { return mStringReport; }
	const BoolMap& getBoolReport() const { return mBoolReport; }

private:
	size_t                              mInitialShapeIndex;
	std::vector<std::vector<double>>    mVertices;
	std::vector<std::vector<uint32_t>>  mFaces;
	FloatMap                            mFloatReport;
	StringMap                           mStringReport;
	BoolMap                             mBoolReport;
};


namespace {

	class ModelGenerator {
	public:
		ModelGenerator(const std::string& initShapePath);
		ModelGenerator(const std::vector<Geometry>& myGeo);
		~ModelGenerator() { }

		std::vector<GeneratedGeometry> generateModel(const std::string& rulePackagePath, py::dict shapeAttributes, py::dict encoderOptions, const std::wstring encoderName);
		std::vector<GeneratedGeometry> generateAnotherModel(py::dict shapeAttributes, py::dict encoderOptions);

	private:
		std::string             mInitialShapePath;
		std::vector<Geometry>   mInitialGeometries;
		pcu::ResolveMapPtr      mResolveMap;
		pcu::CachePtr           mCache;

		pcu::AttributeMapBuilderPtr mEncoderBuilder;
		pcu::AttributeMapPtr        mCGAReportOptions;
		pcu::AttributeMapPtr        mCGAPrintOptions;
		pcu::AttributeMapPtr        mPyEncoderOptions;

		std::vector<pcu::AttributeMapPtr>           mAllEncodersOptionsPtr;
		std::vector<std::wstring>                   mAllEncodersWS;
		std::vector<pcu::InitialShapeBuilderPtr>    mInitialShapesBuilders;

		std::wstring    mRuleFile = L"bin/rule.cgb";
		std::wstring    mStartRule = L"default$init";
		int32_t         mSeed = 666;
		std::wstring    mShapeName = L"InitialShape";

		bool mValid = true;
	};

} // namespace
