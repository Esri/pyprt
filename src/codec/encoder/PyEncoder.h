/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2021 Esri R&D Center Zurich
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

#include "prtx/EncodePreparator.h"
#include "prtx/Encoder.h"
#include "prtx/EncoderFactory.h"
#include "prtx/Singleton.h"
#include "prtx/prtx.h"

#include "prt/AttributeMap.h"
#include "prt/Callbacks.h"

#include <string>

// forward declare some classes to reduce header inclusion
namespace prtx {
class GenerateContext;
}

class PyEncoder : public prtx::GeometryEncoder {
public:
	static const std::wstring ID;
	static const std::wstring NAME;
	static const std::wstring DESCRIPTION;

	using prtx::GeometryEncoder::GeometryEncoder;

	PyEncoder(const PyEncoder&) = delete;
	PyEncoder(PyEncoder&&) = delete;
	PyEncoder& operator=(PyEncoder&) = delete;
	virtual ~PyEncoder() = default;

	virtual void init(prtx::GenerateContext& context) override;
	virtual void encode(prtx::GenerateContext& context, size_t initialShapeIndex) override;
	virtual void finish(prtx::GenerateContext& context) override;

private:
	prtx::DefaultNamePreparator mNamePreparator;
	prtx::EncodePreparatorPtr mEncodePreparator;
};

class PyEncoderFactory : public prtx::EncoderFactory, public prtx::Singleton<PyEncoderFactory> {
public:
	static PyEncoderFactory* createInstance();

	PyEncoderFactory(const prt::EncoderInfo* info) : prtx::EncoderFactory(info) {}
	PyEncoderFactory(const PyEncoderFactory&) = delete;
	PyEncoderFactory(PyEncoderFactory&&) = delete;
	PyEncoderFactory& operator=(PyEncoderFactory&) = delete;
	virtual ~PyEncoderFactory() = default;

	PyEncoder* create(const prt::AttributeMap* defaultOptions, prt::Callbacks* callbacks) const override;
};
