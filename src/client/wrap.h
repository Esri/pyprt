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
	const prt::LogLevel* mLevels;
	size_t mCount;
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

	PythonLogHandler mLogHandler;
	pcu::ObjectPtr mPRTHandle;
};


class Geometry {
public:
	Geometry(const std::vector<double>& vert);
	Geometry() { }
	~Geometry() { }

	void setGeometry(const std::vector<double>& vert, const size_t& vertCnt, const std::vector<uint32_t>& ind, const size_t& indCnt, const std::vector<uint32_t>& faceCnt, const size_t& faceCntCnt);
	const double* getVertices() const { return vertices.data(); }
	size_t getVertexCount() const { return vertexCount; }
	const uint32_t* getIndices() const { return indices.data(); }
	size_t getIndexCount() const { return indexCount; }
	const uint32_t* getFaceCounts() const { return faceCounts.data(); }
	size_t getFaceCountsCount() const { return faceCountsCount; }

protected:
	std::vector<double> vertices;
	size_t vertexCount;
	std::vector<uint32_t> indices;
	size_t indexCount;
	std::vector<uint32_t> faceCounts;
	size_t faceCountsCount;
};

Geometry::Geometry(const std::vector<double>& vert) {
	vertices = vert;
	vertexCount = vert.size();
	indexCount = (size_t)(vertexCount / 3);
	faceCountsCount = 1;

	std::vector<uint32_t> indicesVector(indexCount);
	std::iota(std::begin(indicesVector), std::end(indicesVector), 0);
	indices = indicesVector;
	std::vector<uint32_t> faceVector(1, (uint32_t)indexCount);
	faceCounts = faceVector;
}


class GeneratedGeometry {
public:
	GeneratedGeometry(const size_t& initialShapeIdx, const std::vector<double>& vertMatrix, const std::vector<std::vector<uint32_t>>& fMatrix, const FloatMap& floatRep, const StringMap& stringRep, const BoolMap& boolRep);
	GeneratedGeometry() { }
	~GeneratedGeometry() { }

	void convertGeometryIntoPythonStyle();

	size_t getInitialShapeIndex() const { return initialShapeIdx; }
	std::vector<std::vector<double>> getGenerationVertices() const { return verticesMatrix; }
	std::vector<std::vector<uint32_t>> getGenerationFaces() const { return facesMatrix; }
	FloatMap getGenerationFloatReport() const { return floatReportMap; }
	StringMap getGenerationStringReport() const { return stringReportMap; }
	BoolMap getGenerationBoolReport() const { return boolReportMap; }

private:
	size_t initialShapeIdx;
	std::vector<double> verticesVect;
	std::vector<std::vector<double>> verticesMatrix;
	std::vector<std::vector<uint32_t>> facesMatrix;
	FloatMap floatReportMap;
	StringMap stringReportMap;
	BoolMap boolReportMap;
};


namespace {

	class ModelGenerator {
	public:
		ModelGenerator(const std::string& initShapePath);
		ModelGenerator(const std::vector<Geometry>& myGeo);
		~ModelGenerator() { }

		std::vector<GeneratedGeometry> generateModel(const std::string& rulePackagePath, py::dict shapeAttributes, py::dict encoderOptions, const wchar_t* encoderName);
		std::vector<GeneratedGeometry> generateAnotherModel(py::dict shapeAttributes, py::dict encoderOptions);

	private:
		std::string initialShapePath;
		std::vector<Geometry> initialGeometries;
		pcu::ResolveMapPtr resolveMap;
		pcu::CachePtr cache;

		pcu::AttributeMapBuilderPtr encoderBuilder;
		pcu::AttributeMapPtr CGAReportOptions;
		pcu::AttributeMapPtr CGAPrintOptions;
		pcu::AttributeMapPtr pyEncoderOptions;
		std::vector<const prt::AttributeMap*> allEncodersOptions;
		std::vector<const wchar_t*> allEncoders;

		std::vector<pcu::InitialShapeBuilderPtr> initialShapesBuilders;

		std::wstring ruleFile = L"bin/rule.cgb";
		std::wstring startRule = L"default$init";
		int32_t seed = 666;
		std::wstring shapeName = L"InitialShape";

		bool valid = true;
	};

} // namespace
