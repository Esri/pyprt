/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2020 Esri R&D Center Zurich
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
 * A copy of the license is available in the repository's LICENSE file.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

class InitialShape {
public:
	InitialShape(const std::vector<double>& vert);
	InitialShape(const std::vector<double>& vert, const std::vector<uint32_t>& ind,
	             const std::vector<uint32_t>& faceCnt);
	InitialShape(const std::string& path);
	~InitialShape() = default;

	const double* getVertices() const {
		return mVertices.data();
	}
	size_t getVertexCount() const {
		return mVertices.size();
	}
	const uint32_t* getIndices() const {
		return mIndices.data();
	}
	size_t getIndexCount() const {
		return mIndices.size();
	}
	const uint32_t* getFaceCounts() const {
		return mFaceCounts.data();
	}
	size_t getFaceCountsCount() const {
		return mFaceCounts.size();
	}
	const std::string& getPath() const {
		return mPath;
	}
	bool getPathFlag() const {
		return mPathFlag;
	}

protected:
	const std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaceCounts;
	const std::string mPath;
	const bool mPathFlag;
};
