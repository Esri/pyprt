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


void PyCallbacks::addEntry(const uint32_t initialShapeIndex, const int32_t shapeID, const FloatMap& CGAfloatreport, const StringMap& CGAstringreport, const BoolMap& CGAboolreport, const std::vector<std::vector<double>> verticesCoord, const std::vector<std::vector<uint32_t>> facesCoord) {
    Entry instance = {
        initialShapeIndex,
        shapeID,
        CGAfloatreport,
        CGAstringreport,
        CGAboolreport,
        verticesCoord,
        facesCoord
    };

    shapes.push_back(instance);
}

std::map<int32_t, std::vector<std::vector<double>>> PyCallbacks::getVertices() const {
    std::map<int32_t, std::vector<std::vector<double>>> allVertices;

    for (Entry e : shapes) {
        allVertices[e.id] = e.vertices;
    }

    return allVertices;
}

std::map<int32_t, std::vector<std::vector<uint32_t>>> PyCallbacks::getFaces() const {
    std::map<int32_t, std::vector<std::vector<uint32_t>>> allFaces;

    for (Entry e : shapes) {
        allFaces[e.id] = e.faces;
    }

    return allFaces;
}

std::map<uint32_t, std::map<int32_t, FloatMap>> PyCallbacks::getFloatReport() const {
    std::map<uint32_t,std::map<int32_t, FloatMap>> allFloatReports;
    std::map<int32_t, FloatMap> floatReports;

    //for (Entry e : shapes) {
    //    allFloatReports[e.id] = e.reportFloatData;
    //}
    for (Entry e : shapes) {
        floatReports[e.id] = e.reportFloatData;
        allFloatReports[e.initialShapeIdx] = floatReports;
    }

    return allFloatReports;
}

std::map<int32_t, StringMap> PyCallbacks::getStringReport() const {
    std::map<int32_t, StringMap> allStringReports;

    for (Entry e : shapes) {
        allStringReports[e.id] = e.reportStringData;
    }

    return allStringReports;
}

std::map<int32_t, BoolMap> PyCallbacks::getBoolReport() const {
    std::map<int32_t, BoolMap> allBoolReports;

    for (Entry e : shapes) {
        allBoolReports[e.id] = e.reportBoolData;
    }

    return allBoolReports;
}