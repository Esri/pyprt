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

#include "InitialShape.h"

#include <numeric>

InitialShape::InitialShape(const std::vector<double>& vert) : mVertices(vert), mPathFlag(false) {
	mIndices.resize(vert.size() / 3);
	std::iota(std::begin(mIndices), std::end(mIndices), 0);
	mFaceCounts.resize(1, (uint32_t)mIndices.size());
}

InitialShape::InitialShape(const std::vector<double>& vert, const std::vector<uint32_t>& ind,
                           const std::vector<uint32_t>& faceCnt, const HoleIndices& holes = {{}})
    : mVertices(vert), mIndices(ind), mFaceCounts(faceCnt), mPathFlag(false) {

	for (auto& holesPerFaceWithHoles : holes) {
		mHoles.insert(mHoles.end(), holesPerFaceWithHoles.begin(), holesPerFaceWithHoles.end());
		mHoles.push_back(UINT32_MAX);
	}
}

InitialShape::InitialShape(const std::string& initShapePath) : mPath(initShapePath), mPathFlag(true) {}
