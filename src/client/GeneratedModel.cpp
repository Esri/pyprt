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

#include "GeneratedModel.h"

GeneratedModel::GeneratedModel(const size_t& initShapeIdx, GeneratedPayloadPtr payload)
    : mInitialShapeIndex(initShapeIdx), mPayload(payload) {}

size_t GeneratedModel::getInitialShapeIndex() const {
	return mInitialShapeIndex;
}
const Coordinates& GeneratedModel::getVertices() const {
	return mPayload->mVertices;
}
const Indices& GeneratedModel::getIndices() const {
	return mPayload->mIndices;
}
const Indices& GeneratedModel::getFaces() const {
	return mPayload->mFaces;
}
const pybind11::dict& GeneratedModel::getReport() const {
	return mPayload->mCGAReport;
}
const std::wstring& GeneratedModel::getCGAPrints() const {
	return mPayload->mCGAPrints;
}
const std::vector<std::wstring>& GeneratedModel::getCGAErrors() const {
	return mPayload->mCGAErrors;
}
const pybind11::dict& GeneratedModel::getAttributes() const {
	return mPayload->mAttrVal;
}
