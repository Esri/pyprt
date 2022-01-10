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

#include "GeneratedPayload.h"
#include "types.h"

#include "pybind11/pybind11.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class GeneratedModel {
public:
	GeneratedModel() = default;
	explicit GeneratedModel(const size_t& initialShapeIdx, GeneratedPayloadPtr payload);
	~GeneratedModel() = default;

	size_t getInitialShapeIndex() const;
	const Coordinates& getVertices() const;
	const Indices& getIndices() const;
	const Indices& getFaces() const;
	const pybind11::dict& getReport() const;
	const std::wstring& getCGAPrints() const;
	const std::vector<std::wstring>& getCGAErrors() const;
	const pybind11::dict& getAttributes() const;

private:
	size_t mInitialShapeIndex;
	GeneratedPayloadPtr mPayload;
};

PYBIND11_MAKE_OPAQUE(std::vector<GeneratedModel>);
