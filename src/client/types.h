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

#include "prt/API.h"
#include "prt/FileOutputCallbacks.h"

#include <cstdint>
#include <memory>
#include <vector>

using Coordinates = std::vector<double>;
using Indices = std::vector<uint32_t>;
using HoleIndices = std::vector<Indices>;

/**
 * helpers for prt object management
 */
struct PRTDestroyer {
	void operator()(const prt::Object* p) const {
		if (p)
			p->destroy();
	}
};

using ObjectPtr = std::unique_ptr<const prt::Object, PRTDestroyer>;
using CachePtr = std::unique_ptr<prt::CacheObject, PRTDestroyer>;
using ResolveMapPtr = std::unique_ptr<const prt::ResolveMap, PRTDestroyer>;
using ResolveMapBuilderPtr = std::unique_ptr<prt::ResolveMapBuilder, PRTDestroyer>;
using InitialShapePtr = std::unique_ptr<const prt::InitialShape, PRTDestroyer>;
using InitialShapeBuilderPtr = std::unique_ptr<prt::InitialShapeBuilder, PRTDestroyer>;
using AttributeMapPtr = std::unique_ptr<const prt::AttributeMap, PRTDestroyer>;
using AttributeMapBuilderPtr = std::unique_ptr<prt::AttributeMapBuilder, PRTDestroyer>;
using FileOutputCallbacksPtr = std::unique_ptr<prt::FileOutputCallbacks, PRTDestroyer>;
using ConsoleLogHandlerPtr = std::unique_ptr<prt::ConsoleLogHandler, PRTDestroyer>;
using FileLogHandlerPtr = std::unique_ptr<prt::FileLogHandler, PRTDestroyer>;
using EncoderInfoPtr = std::unique_ptr<const prt::EncoderInfo, PRTDestroyer>;
using DecoderInfoPtr = std::unique_ptr<const prt::DecoderInfo, PRTDestroyer>;
using SimpleOutputCallbacksPtr = std::unique_ptr<prt::SimpleOutputCallbacks, PRTDestroyer>;
using RuleFileInfoUPtr = std::unique_ptr<const prt::RuleFileInfo, PRTDestroyer>;
