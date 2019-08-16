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


std::map<uint32_t, std::vector<std::vector<double>>> PyCallbacks::getVertices() const {
    return verticesMap;
}

std::map<uint32_t, std::vector<std::vector<uint32_t>>> PyCallbacks::getFaces() const {
    return facesMap;
}

std::map<uint32_t, FloatMap> PyCallbacks::getFloatReport() const {
    return CGAfloatReportsMap;
}

std::map<uint32_t, StringMap> PyCallbacks::getStringReport() const {
    return CGAstringReportsMap;
}

std::map<uint32_t, BoolMap> PyCallbacks::getBoolReport() const {
    return CGAboolReportsMap;
}