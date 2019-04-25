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


void PyCallbacks::add(const wchar_t* name, const int32_t shapeIDs) { }

void PyCallbacks::setReports(const FloatMap& CGAfloatreport, const StringMap& CGAstringreport, const BoolMap& CGAboolreport) {
    reportFloatData = CGAfloatreport;
    reportStringData = CGAstringreport;
    reportBoolData = CGAboolreport;
}

void PyCallbacks::setVertices(const std::vector<std::vector<double>> verticesCoord) {
    vertices = verticesCoord;
}

void PyCallbacks::setFaces(const std::vector<std::vector<uint32_t>> facesCoord) {
    faces = facesCoord;
}