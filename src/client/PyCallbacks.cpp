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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

bool PyCallbacks::isHiddenAttribute(const RuleFileInfoUPtr& ruleFileInfo, const wchar_t* key) {
	for (size_t ai = 0, numAttrs = ruleFileInfo->getNumAttributes(); ai < numAttrs; ai++) {
		const auto attr = ruleFileInfo->getAttribute(ai);
		if (std::wcscmp(key, attr->getName()) == 0) {
			for (size_t k = 0, numAnns = attr->getNumAnnotations(); k < numAnns; k++) {
				if (std::wcscmp(attr->getAnnotation(k)->getName(), L"@Hidden") == 0)
					return true;
			}
			return false;
		}
	}
	return false;
}

std::wstring PyCallbacks::removeDefaultStyleName(const wchar_t* key) {
	const std::wstring keyName = key;
	if (keyName.find(L"Default$") == 0)
		return keyName.substr(8);
	else
		return keyName;
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
