/**
 * Esri CityEngine SDK Callbacks for Geometry Encoder for Python
 *
 * This example demonstrates the usage of the PRTX interface
 * to write custom encoders.
 *
 * See README.md in http://github.com/ArcGIS/esri-cityengine-sdk for build instructions.
 *
 * Written by Camille Lechot
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


#pragma once

#include "IPyCallbacks.h"

#include "prt/Callbacks.h"

#include <string>
#include <vector>
#include <iostream>
#include <map>


class PyCallbacks : public IPyCallbacks {
private:
    std::vector<std::vector<double>> vertices;
    std::vector<std::vector<uint32_t>> faces;
    FloatMap reportFloatData;
    StringMap reportStringData;
    BoolMap reportBoolData;

public:
    
	PyCallbacks() = default;

	virtual ~PyCallbacks() = default;

    void add(
        const wchar_t* name,
        const int32_t shapeID
    ) override;

    void setReports(
        const FloatMap& CGAfloatreport,
        const StringMap& CGAstringreport,
        const BoolMap& CGAboolreport
    ) override;

    void setVertices(
        const std::vector<std::vector<double>> verticesCoord
    ) override;

    void setFaces(
        const std::vector<std::vector<uint32_t>> facesCoord
    ) override;

    std::vector<std::vector<double>> getVertices() const {
        return vertices;
    }

    std::vector<std::vector<uint32_t>> getFaces() const {
        return faces;
    }

    std::map<std::string, float> getFloatReport() const {
        return reportFloatData;
    }

    std::map<std::string, std::string> getStringReport() const {
        return reportStringData;
    }

    std::map<std::string, bool> getBoolReport() const {
        return reportBoolData;
    }

	prt::Status generateError(size_t isIndex, prt::Status status, const wchar_t* message) {
        std::wcout << "GENERATE ERROR: " << message << std::endl;
		return prt::STATUS_OK;
	}

	prt::Status assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri, const wchar_t* message) {
        std::wcout << "ASSET ERROR: " << message << std::endl;
		return prt::STATUS_OK;
	}

	prt::Status cgaError(size_t isIndex, int32_t shapeID, prt::CGAErrorLevel level, int32_t methodId, int32_t pc, const wchar_t* message) {
        std::wcout << "CGA ERROR: " << message << std::endl;
        return prt::STATUS_OK;
	}

	prt::Status cgaPrint(size_t isIndex, int32_t shapeID, const wchar_t* txt) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportBool(size_t isIndex, int32_t shapeID, const wchar_t* key, bool value) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportFloat(size_t isIndex, int32_t shapeID, const wchar_t* key, double value) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportString(size_t isIndex, int32_t shapeID, const wchar_t* key, const wchar_t* value) {
		return prt::STATUS_OK;
	}

	prt::Status attrBool(size_t isIndex, int32_t shapeID, const wchar_t* key, bool value) {
		return prt::STATUS_OK;
	}

	prt::Status attrFloat(size_t isIndex, int32_t shapeID, const wchar_t* key, double value) {
		return prt::STATUS_OK;
	}

	prt::Status attrString(size_t isIndex, int32_t shapeID, const wchar_t* key, const wchar_t* value) {
		return prt::STATUS_OK;
	}


};
