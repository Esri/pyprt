/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2024 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * A copy of the license is available in the repository's LICENSE file.
 */

#pragma once

#include "codec.h"
#include "prt/Callbacks.h"

class PYENC_EXPORTS_API IPyCallbacks : public prt::Callbacks {
public:
	virtual ~IPyCallbacks() override = default;

	virtual void addGeometry(const size_t initialShapeIndex, const double* vertexCoords, const size_t vextexCoordsCount,
	                         const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
	                         const size_t faceCountsCount) = 0;

	virtual void addReports(const size_t initialShapeIndex, const wchar_t** stringReportKeys,
	                        const wchar_t** stringReportValues, size_t stringReportCount,
	                        const wchar_t** floatReportKeys, const double* floatReportValues, size_t floatReportCount,
	                        const wchar_t** boolReportKeys, const bool* boolReportValues, size_t boolReportCount) = 0;
};
