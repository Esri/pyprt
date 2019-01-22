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

#include "prt/Callbacks.h"

#include <string>


class IPyCallbacks : public prt::Callbacks {
public:

    virtual ~IPyCallbacks() override = default;

    /**
     * @param uvCounts uv index count per face in groups of "uvSets" (f0 uvset 0, f0 uvset 1, ..., f1 uvset 0, ...)
     * @param uvIndices all uv indices of all uv sets
     * @param uvSets number of uv sets
     * @param faceRanges ranges for materials and reports
     * @param materials contains faceRangesSize-1 attribute maps (all materials must have an identical set of keys and types)
     * @param reports contains faceRangesSize-1 attribute maps
     * @param shapeIDs shape ids per face, contains faceRangesSize-1 values
     */
    virtual void add(
        const wchar_t* name,
        const std::vector<std::vector<double>> verticesCoord,
        //const prt::AttributeMap** reports,
        const int32_t* shapeIDs
    ) = 0;
};
