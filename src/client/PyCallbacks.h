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

#pragma once

#include "types.h"
#include "utils.h"

#include "encoder/IPyCallbacks.h"

#include "prt/Callbacks.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace py = pybind11;

class PyCallbacks;
using PyCallbacksPtr = std::unique_ptr<PyCallbacks>;

const std::wstring ERRORLEVELS[] = {L"Error ", L"Warning ", L"Info "};

struct Model {
	Coordinates mVertices;
	Indices mIndices;
	Indices mFaces;
	py::dict mCGAReport;
	std::wstring mCGAPrints;
	std::vector<std::wstring> mCGAErrors;
	py::dict mAttrVal;
};

class PyCallbacks : public IPyCallbacks {
private:
	std::vector<Model> mModels;
	std::unordered_set<std::wstring> mHiddenAttrs;

public:
	PyCallbacks(const size_t initialShapeCount, const std::unordered_set<std::wstring>& hiddenAttrs) {
		mModels.resize(initialShapeCount);
		mHiddenAttrs = hiddenAttrs;
	}

	virtual ~PyCallbacks() = default;

	bool isHiddenAttribute(const wchar_t* key);

	void addGeometry(const size_t initialShapeIndex, const double* vertexCoords, const size_t vextexCoordsCount,
	                 const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
	                 const size_t faceCountsCount) override;

	void addReports(const size_t initialShapeIndex, const wchar_t** stringReportKeys,
	                const wchar_t** stringReportValues, size_t stringReportCount, const wchar_t** floatReportKeys,
	                const double* floatReportValues, size_t floatReportCount, const wchar_t** boolReportKeys,
	                const bool* boolReportValues, size_t boolReportCount) override;

	size_t getInitialShapeCount() const {
		return mModels.size();
	}

	const Coordinates& getVertices(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mVertices;
	}

	const Indices& getIndices(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mIndices;
	}

	const Indices& getFaces(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mFaces;
	}

	const py::dict& getReport(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mCGAReport;
	}

	const std::wstring& getCGAPrints(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mCGAPrints;
	}

	const std::vector<std::wstring>& getCGAErrors(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mCGAErrors;
	}

	const py::dict& getAttributes(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mAttrVal;
	}

	prt::Status generateError(size_t /*isIndex*/, prt::Status /*status*/, const wchar_t* /*message*/) override {
		return prt::STATUS_OK;
	}

	prt::Status assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri,
	                       const wchar_t* message) override {
		std::wstring errorMsg(L"Asset" + ERRORLEVELS[level] + key + L" " + uri + L"\n" + message);
		mModels[isIndex].mCGAErrors.push_back(errorMsg);

		return prt::STATUS_OK;
	}

	prt::Status cgaError(size_t isIndex, int32_t /*shapeID*/, prt::CGAErrorLevel level, int32_t /*methodId*/,
	                     int32_t /*pc*/, const wchar_t* message) override {
		std::wstring errorMsg(L"CGA" + ERRORLEVELS[level] + L"\n" + message);
		mModels[isIndex].mCGAErrors.push_back(errorMsg);

		return prt::STATUS_OK;
	}

	prt::Status cgaPrint(size_t isIndex, int32_t /*shapeID*/, const wchar_t* txt) override {
		std::wstring printsTxt(txt);
		mModels[isIndex].mCGAPrints += printsTxt;

		return prt::STATUS_OK;
	}

	prt::Status cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                          bool /*value*/) override {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                           double /*value*/) override {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                            const wchar_t* /*value*/) override {
		return prt::STATUS_OK;
	}

	prt::Status attrBool(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, bool value) override;

	prt::Status attrFloat(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, double value) override;

	prt::Status attrString(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const wchar_t* value) override;

	prt::Status attrBoolArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const bool* ptr,
	                          size_t size, size_t nRows) override;

	prt::Status attrFloatArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const double* ptr,
	                           size_t size, size_t nRows) override;

	prt::Status attrStringArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
	                            const wchar_t* const* ptr, size_t size, size_t nRows) override;

	template <typename T>
	prt::Status storeAttr(size_t isIndex, const wchar_t* key, const T value) {
		if (!isHiddenAttribute(key)) {
			py::object pyKey = py::cast(pcu::removeDefaultStyleName(key));
			mModels[isIndex].mAttrVal[pyKey] = value;
		}

		return prt::STATUS_OK;
	}

	template <typename T>
	prt::Status storeAttr(size_t isIndex, const wchar_t* key, const T* ptr, const size_t size,
	                                   const size_t nRows) {
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
};
