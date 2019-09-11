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


#pragma once

#include "IPyCallbacks.h"

#include "prt/Callbacks.h"

#include <pybind11/pybind11.h>

#include <string>
#include <vector>
#include <iostream>
#include <map>


using FloatMap = std::map<std::wstring, double>;
using StringMap = std::map<std::wstring, std::wstring>;
using BoolMap = std::map<std::wstring, bool>;


class PyCallbacks : public IPyCallbacks {
private:
    struct Model {
        FloatMap                            mCGAFloatReport;
        StringMap                           mCGAStringReport;
        BoolMap                             mCGABoolReport;
        std::vector<double>                 mVertices;
        std::vector<std::vector<uint32_t>>  mFaces;
    };

    std::vector<Model> mModels;

public:
    
    PyCallbacks(const size_t initialShapeCount) { mModels.resize(initialShapeCount); }

	virtual ~PyCallbacks() = default;

    void addGeometry(
        const size_t initialShapeIndex,
        const double* vertexCoords,
        const size_t vextexCoordsCount,
        const uint32_t* facesIndices,
        const uint32_t* faceCounts,
        const size_t faceCountsCount
    ) override;

    void addReports(
        const size_t initialShapeIndex,
        const wchar_t** stringReportKeys,
        const wchar_t** stringReportValues,
        size_t stringReportCount,
        const wchar_t** floatReportKeys,
        const double* floatReportValues,
        size_t floatReportCount,
        const wchar_t** boolReportKeys,
        const bool* boolReportValues,
        size_t boolReportCount
    ) override;

    size_t getInitialShapeCount() const { return mModels.size(); }

    const std::vector<double>& getVertices(const size_t initialShapeIdx) const { return mModels[initialShapeIdx].mVertices; }

    const std::vector<std::vector<uint32_t>>& getFaces(const size_t initialShapeIdx) const { return mModels[initialShapeIdx].mFaces; }

    const FloatMap& getFloatReport(const size_t initialShapeIdx) const { return mModels[initialShapeIdx].mCGAFloatReport; }

    const StringMap& getStringReport(const size_t initialShapeIdx) const { return mModels[initialShapeIdx].mCGAStringReport; }

    const BoolMap& getBoolReport(const size_t initialShapeIdx) const { return mModels[initialShapeIdx].mCGABoolReport; }

	prt::Status generateError(size_t isIndex, prt::Status status, const wchar_t* message) {
		pybind11::print(L"GENERATE ERROR:", isIndex, status, message);
		return prt::STATUS_OK;
	}

	prt::Status assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri, const wchar_t* message) {
		pybind11::print(L"ASSET ERROR:", isIndex, level, key, uri, message);
		return prt::STATUS_OK;
	}

	prt::Status cgaError(size_t isIndex, int32_t shapeID, prt::CGAErrorLevel level, int32_t methodId, int32_t pc, const wchar_t* message) {
		pybind11::print(L"CGA ERROR:", isIndex, shapeID, level, methodId, pc, message);
        return prt::STATUS_OK;
	}

	prt::Status cgaPrint(size_t isIndex, int32_t shapeID, const wchar_t* txt) {
		pybind11::print(L"CGA PRINT:", isIndex, shapeID, txt);
		return prt::STATUS_OK;
	}

	prt::Status cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, double /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const wchar_t* /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, double /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const wchar_t* /*value*/) {
		return prt::STATUS_OK;
	}
};
