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


void PyCallbacks::addGeometry(const uint32_t initialShapeIndex, const std::vector<std::vector<double>>& verticesCoord, const std::vector<std::vector<uint32_t>>& facesCoord) {
    
    if (!verticesCoord.empty()) {
        auto it = verticesMap.find(initialShapeIndex);

        if (it != verticesMap.end()) {
            std::vector<std::vector<double>> existVertMat = verticesMap.at(initialShapeIndex);
            existVertMat.insert(existVertMat.begin(), verticesCoord.begin(), verticesCoord.end());
            verticesMap.at(initialShapeIndex) = existVertMat;
        }
        else {
            initialShapesIndices.insert(initialShapeIndex);
            verticesMap.insert({ initialShapeIndex, verticesCoord });
        }
    }

    if (!facesCoord.empty()) {
        auto it = facesMap.find(initialShapeIndex);

        if (it != facesMap.end()) {
            std::vector<std::vector<uint32_t>> existFacesMat = facesMap.at(initialShapeIndex);
            existFacesMat.insert(existFacesMat.begin(), facesCoord.begin(), facesCoord.end());
            facesMap.at(initialShapeIndex) = existFacesMat;
        }
        else {
            initialShapesIndices.insert(initialShapeIndex);
            facesMap.insert({ initialShapeIndex, facesCoord });
        }
    }
}

void PyCallbacks::addReports(const uint32_t initialShapeIndex, const FloatMap& CGAfloatreport, const StringMap& CGAstringreport, const BoolMap& CGAboolreport) {
    initialShapesIndices.insert(initialShapeIndex);
    CGAfloatReportsMap.insert({ initialShapeIndex, CGAfloatreport });
    CGAstringReportsMap.insert({ initialShapeIndex, CGAstringreport });
    CGAboolReportsMap.insert({ initialShapeIndex, CGAboolreport });
}

void PyCallbacks::addIndex(const uint32_t initialShapeIndex) {
    initialShapesIndices.insert(initialShapeIndex);
}

uint32_t PyCallbacks::getInitialShapeIndex(size_t i) const {
    if (initialShapesIndices.empty())
        return -1;

    auto it = initialShapesIndices.begin();
    std::advance(it, i);
    return *it;
}


std::vector<std::vector<double>> PyCallbacks::getVertices(uint32_t index) const {
    if ((verticesMap.find(index) == verticesMap.end()) || (verticesMap.empty()))
        return {};
    else
        return verticesMap.at(index);
}

std::vector<std::vector<uint32_t>> PyCallbacks::getFaces(uint32_t index) const {
    if ((facesMap.find(index) == facesMap.end()) || (facesMap.empty()))
        return {};
    else
        return facesMap.at(index);
}

FloatMap PyCallbacks::getFloatReport(uint32_t index) const {
    if ((CGAfloatReportsMap.find(index) == CGAfloatReportsMap.end()) || (CGAfloatReportsMap.empty()))
        return {};
    else
        return CGAfloatReportsMap.at(index);
}

StringMap PyCallbacks::getStringReport(uint32_t index) const {
    if ((CGAstringReportsMap.find(index) == CGAstringReportsMap.end()) || (CGAstringReportsMap.empty()))
        return {};
    else
        return CGAstringReportsMap.at(index);
}

BoolMap PyCallbacks::getBoolReport(uint32_t index) const {
    if ((CGAboolReportsMap.find(index) == CGAboolReportsMap.end()) || (CGAboolReportsMap.empty()))
        return {};
    else
        return CGAboolReportsMap.at(index);
}