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


#include "PyCallbacks.h"

#include <string>
#include <vector>
#include <map>
#include <tuple>


void PyCallbacks::addGeometry(const uint32_t initialShapeIndex, const std::vector<std::vector<double>> verticesCoord, const std::vector<std::vector<uint32_t>> facesCoord) {
    std::map<uint32_t, std::vector<std::vector<double>>>::iterator it;
    it = verticesMap.find(initialShapeIndex);

    if (it != verticesMap.end()) {
        std::vector<std::vector<double>> existVertMat = verticesMap.at(initialShapeIndex);
        existVertMat.insert(existVertMat.begin(), verticesCoord.begin(), verticesCoord.end());
        verticesMap.at(initialShapeIndex) = existVertMat;

        std::vector<std::vector<uint32_t>> existFacesMat = facesMap.at(initialShapeIndex);
        existFacesMat.insert(existFacesMat.begin(), facesCoord.begin(), facesCoord.end());
        facesMap.at(initialShapeIndex) = existFacesMat;
    }
    else {
        initialShapesIndices.insert(initialShapeIndex);
        verticesMap.insert({ initialShapeIndex, verticesCoord });
        facesMap.insert({ initialShapeIndex, facesCoord });
    }
}

void PyCallbacks::addReports(const uint32_t initialShapeIndex, const FloatMap& CGAfloatreport, const StringMap& CGAstringreport, const BoolMap& CGAboolreport) {
    initialShapesIndices.insert(initialShapeIndex);
    CGAfloatReportsMap.insert({ initialShapeIndex, CGAfloatreport });
    CGAstringReportsMap.insert({ initialShapeIndex, CGAstringreport });
    CGAboolReportsMap.insert({ initialShapeIndex, CGAboolreport });
}

std::unordered_set<uint32_t> PyCallbacks::getInitialShapesIndices() const {
    return initialShapesIndices;
}


std::vector<std::tuple<uint32_t, int32_t, std::vector<std::vector<double>>>> PyCallbacks::getVertices() const {
    std::vector<std::tuple<uint32_t, int32_t, std::vector<std::vector<double>>>> allVertices;

    for (auto i : initialShapesIndices) {
        std::tuple<uint32_t, int32_t, std::vector<std::vector<double>>> vertMat(i, 10, verticesMap.at(i));
        allVertices.push_back(vertMat);
    }

    return allVertices;
}

std::vector<std::tuple<uint32_t, int32_t, std::vector<std::vector<uint32_t>>>> PyCallbacks::getFaces() const {
    std::vector<std::tuple<uint32_t, int32_t, std::vector<std::vector<uint32_t>>>> allFaces;

    for (auto i : initialShapesIndices) {
        std::tuple<uint32_t, int32_t, std::vector<std::vector<uint32_t>>> facesMat(i, 10, facesMap.at(i));
        allFaces.push_back(facesMat);
    }

    return allFaces;
}

std::map<uint32_t, FloatMap> PyCallbacks::getFloatReportNEW() const {
    return CGAfloatReportsMap;
}

std::vector<std::tuple<uint32_t, int32_t, FloatMap>> PyCallbacks::getFloatReport() const {
    std::vector<std::tuple<uint32_t, int32_t, FloatMap>> allFloatReports;

    if (CGAfloatReportsMap.size() > 0) {
        for (auto i : initialShapesIndices) {
            std::tuple<uint32_t, int32_t, FloatMap> floatReport(i, 10, CGAfloatReportsMap.at(i));
            allFloatReports.push_back(floatReport);
        }
    }

    return allFloatReports;
}

std::vector<std::tuple<uint32_t, int32_t, StringMap>> PyCallbacks::getStringReport() const {
    std::vector<std::tuple<uint32_t, int32_t, StringMap>> allStringReports;

    if (CGAstringReportsMap.size() > 0) {
        for (auto i : initialShapesIndices) {
            std::tuple<uint32_t, int32_t, StringMap> stringReport(i, 10, CGAstringReportsMap.at(i));
            allStringReports.push_back(stringReport);
        }
    }

    return allStringReports;
}

std::vector<std::tuple<uint32_t, int32_t, BoolMap>> PyCallbacks::getBoolReport() const {
    std::vector<std::tuple<uint32_t, int32_t, BoolMap>> allBoolReports;

    if (CGAboolReportsMap.size() > 0) {
        for (auto i : initialShapesIndices) {
            std::tuple<uint32_t, int32_t, BoolMap> boolReport(i, 10, CGAboolReportsMap.at(i));
            allBoolReports.push_back(boolReport);
        }
    }

    return allBoolReports;
}