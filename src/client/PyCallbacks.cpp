/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
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

#include "PyCallbacks.h"
#include "utils.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename T>
prt::Status PyCallbacks::storeAttr(size_t isIndex, const wchar_t* key, const T value) {
	if (!isHiddenAttribute(key)) {
		py::object pyKey = py::cast(pcu::removeDefaultStyleName(key));
		mModels[isIndex].mAttrVal[pyKey] = value;
	}

	return prt::STATUS_OK;
}

template <typename T>
prt::Status PyCallbacks::storeAttr(size_t isIndex, const wchar_t* key, const T* ptr, const size_t size, const size_t nRows) {
	if (!isHiddenAttribute(key)) {
		py::object pyKey = py::cast(pcu::removeDefaultStyleName(key));
		const size_t nCol = size / nRows;

		if (nRows > 1) {
			std::vector<std::vector<T>> values(nRows, std::vector<T>(nCol));
			for (size_t i = 0; i < size; i++) {
				const size_t j = i / nCol;
				const size_t k = i % nCol;
				values[j][k] = ptr[i];
			}

			mModels[isIndex].mAttrVal[pyKey] = values;
			return prt::STATUS_OK;
		}
		else {
			std::vector<T> values(nCol);
			for (size_t i = 0; i < size; i++)
				values[i] = ptr[i];

			mModels[isIndex].mAttrVal[pyKey] = values;
			return prt::STATUS_OK;
		}
	}

	return prt::STATUS_OK;
}


bool PyCallbacks::isHiddenAttribute(const wchar_t* key) {
	if (key != nullptr) {
		std::unordered_set<std::wstring>::iterator it = std::find(mHiddenAttrs.begin(), mHiddenAttrs.end(), key);
		if (it != mHiddenAttrs.end())
			return true;
	}
	
	return false;
}

void PyCallbacks::addGeometry(const size_t initialShapeIndex, const double* vertexCoords,
                              const size_t vertexCoordsCount, const uint32_t* faceIndices,
                              const size_t faceIndicesCount, const uint32_t* faceCounts, const size_t faceCountsCount) {

	Model& currentModel = mModels[initialShapeIndex];

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

	Model& currentModel = mModels[initialShapeIndex];

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

prt::Status PyCallbacks::attrBool(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, bool value) {
	return storeAttr(isIndex, key, value);
}

prt::Status PyCallbacks::attrFloat(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, double value) {
	return storeAttr(isIndex, key, value);
}

prt::Status PyCallbacks::attrString(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                    const wchar_t* value) {
	return storeAttr(isIndex, key, value);
}

prt::Status PyCallbacks::attrBoolArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                       const bool* ptr, size_t size, size_t nRows) {
	return storeAttr(isIndex, key, ptr, size, nRows);
}

prt::Status PyCallbacks::attrFloatArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                        const double* ptr, size_t size, size_t nRows) {
	return storeAttr(isIndex, key, ptr, size, nRows);
}

prt::Status PyCallbacks::attrStringArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                            const wchar_t* const* ptr, size_t size, size_t nRows) {
	return storeAttr(isIndex, key, ptr, size, nRows);
}
