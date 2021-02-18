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

#include "GeneratedModel.h"

GeneratedModel::GeneratedModel(const size_t& initShapeIdx, const Coordinates& vert, const Indices& indices,
                               const Indices& face, const pybind11::dict& rep, const std::wstring& cgaPrints,
                               const std::vector<std::wstring>& cgaErrors, const pybind11::dict& attrVal)
    : mInitialShapeIndex(initShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mReport(rep),
      mCGAPrints(cgaPrints), mCGAErrors(cgaErrors), mAttributes(attrVal) {}
