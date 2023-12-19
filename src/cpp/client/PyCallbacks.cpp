/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2023 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * A copy of the license is available in the repository's LICENSE file.
 */

#include "PyCallbacks.h"

PyCallbacks::PyCallbacks(const size_t initialShapeCount, const std::unordered_set<std::wstring>& hiddenAttrs) {
	mPayloads.resize(initialShapeCount);
	mHiddenAttrs = hiddenAttrs;
}

prt::Status PyCallbacks::generateError(size_t /*isIndex*/, prt::Status /*status*/, const wchar_t* /*message*/) {
	return prt::STATUS_OK;
}

prt::Status PyCallbacks::assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri,
                                    const wchar_t* message) {
	std::wstring errorMsg(L"Asset" + ERRORLEVELS[level] + key + L" " + uri + L"\n" + message);
	getOrCreate(isIndex).mCGAErrors.push_back(errorMsg);

	return prt::STATUS_OK;
}

prt::Status PyCallbacks::cgaError(size_t isIndex, int32_t /*shapeID*/, prt::CGAErrorLevel level, int32_t /*methodId*/,
                                  int32_t /*pc*/, const wchar_t* message) {
	std::wstring errorMsg(L"CGA" + ERRORLEVELS[level] + L"\n" + message);
	getOrCreate(isIndex).mCGAErrors.push_back(errorMsg);

	return prt::STATUS_OK;
}

prt::Status PyCallbacks::cgaPrint(size_t isIndex, int32_t /*shapeID*/, const wchar_t* txt) {
	std::wstring printsTxt(txt);
	getOrCreate(isIndex).mCGAPrints += printsTxt;

	return prt::STATUS_OK;
}

prt::Status PyCallbacks::cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                       bool /*value*/) {
	return prt::STATUS_OK;
}

prt::Status PyCallbacks::cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                        double /*value*/) {
	return prt::STATUS_OK;
}

prt::Status PyCallbacks::cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                         const wchar_t* /*value*/) {
	return prt::STATUS_OK;
}

prt::Status PyCallbacks::attrBool(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, bool value) {
	return storeAttr(isIndex, key, value);
}

prt::Status PyCallbacks::attrFloat(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, double value) {
	return storeAttr(isIndex, key, value);
}

prt::Status PyCallbacks::attrString(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const wchar_t* value) {
	return storeAttr(isIndex, key, value);
}

prt::Status PyCallbacks::attrBoolArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const bool* ptr,
                                       size_t size, size_t nRows) {
	return storeAttr(isIndex, key, ptr, size, nRows);
}

prt::Status PyCallbacks::attrFloatArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const double* ptr,
                                        size_t size, size_t nRows) {
	return storeAttr(isIndex, key, ptr, size, nRows);
}

prt::Status PyCallbacks::attrStringArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                         const wchar_t* const* ptr, size_t size, size_t nRows) {
	return storeAttr(isIndex, key, ptr, size, nRows);
}

void PyCallbacks::addGeometry(const size_t initialShapeIndex, const double* vertexCoords,
                              const size_t vertexCoordsCount, const uint32_t* faceIndices,
                              const size_t faceIndicesCount, const uint32_t* faceCounts, const size_t faceCountsCount) {

	GeneratedPayload& currentModel = getOrCreate(initialShapeIndex);

	if (vertexCoords != nullptr)
		currentModel.mVertices.insert(currentModel.mVertices.end(), vertexCoords, vertexCoords + vertexCoordsCount);

	if (faceIndices != nullptr)
		currentModel.mIndices.insert(currentModel.mIndices.end(), faceIndices, faceIndices + faceIndicesCount);

	if (faceCounts != nullptr)
		currentModel.mFaces.insert(currentModel.mFaces.end(), faceCounts, faceCounts + faceCountsCount);
}

void PyCallbacks::addReports(const size_t initialShapeIndex, const wchar_t** stringReportKeys,
                             const wchar_t** stringReportValues, size_t stringReportCount,
                             const wchar_t** floatReportKeys, const double* floatReportValues, size_t floatReportCount,
                             const wchar_t** boolReportKeys, const bool* boolReportValues, size_t boolReportCount) {
	namespace py = pybind11;

	GeneratedPayload& currentModel = getOrCreate(initialShapeIndex);

	for (size_t i = 0; i < boolReportCount; i++) {
		py::object pyKey = py::cast(boolReportKeys[i]);
		currentModel.mCGAReport[pyKey] = boolReportValues[i];
	}

	for (size_t i = 0; i < floatReportCount; i++) {
		py::object pyKey = py::cast(floatReportKeys[i]);
		currentModel.mCGAReport[pyKey] = floatReportValues[i];
	}

	for (size_t i = 0; i < stringReportCount; i++) {
		py::object pyKey = py::cast(stringReportKeys[i]);
		currentModel.mCGAReport[pyKey] = stringReportValues[i];
	}
}

GeneratedPayloadPtr PyCallbacks::getGeneratedPayload(size_t initialShapeIndex) {
	if (initialShapeIndex >= mPayloads.size())
		throw std::out_of_range("initial shape index is out of range.");
	return mPayloads[initialShapeIndex];
}

bool PyCallbacks::isHiddenAttribute(const wchar_t* key) {
	if (key != nullptr) {
		auto it = std::find(mHiddenAttrs.begin(), mHiddenAttrs.end(), key);
		if (it != mHiddenAttrs.end())
			return true;
	}

	return false;
}

GeneratedPayload& PyCallbacks::getOrCreate(size_t initialShapeIndex) {
	assert(mPayloads.size() > initialShapeIndex);
	if (!mPayloads[initialShapeIndex]) {
		mPayloads[initialShapeIndex] = std::make_shared<GeneratedPayload>();
	}
	return *mPayloads[initialShapeIndex];
}
