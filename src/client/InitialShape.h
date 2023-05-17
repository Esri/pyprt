/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2022 Esri R&D Center Zurich
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

#include "types.h"

#include <cstdint>
#include <string>
#include <vector>

class InitialShape {
public:
	explicit InitialShape(const Coordinates& vert);
	explicit InitialShape(const Coordinates& vert, const Indices& ind, const Indices& faceCnt,
	                      const HoleIndices& holes);
	explicit InitialShape(const std::string& path, uint8_t directoryRecursionDepth = 0);
	~InitialShape() = default;

	const double* getVertices() const;
	size_t getVertexCount() const;
	const uint32_t* getIndices() const;
	size_t getIndexCount() const;
	const uint32_t* getFaceCounts() const;
	size_t getFaceCountsCount() const;
	const uint32_t* getHoles() const;
	size_t getHolesCount() const;
	const std::string& getPath() const;
	bool initializedFromPath() const;
	uint8_t getDirectoryRecursionDepth() const;

protected:
	const Coordinates mVertices;
	Indices mIndices;
	Indices mFaceCounts;
	Indices mHoles;
	const std::string mPath;
	const uint8_t mDirectoryRecursionDepth = 0;
};
