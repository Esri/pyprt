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

#include "PythonLogHandler.h"
#include "types.h"

#include "prt/LogLevel.h"

#include <memory>


/**
 * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
 */
struct PRTContext {
	PRTContext(prt::LogLevel minimalLogLevel);
	~PRTContext();

	PythonLogHandler mLogHandler;
	ObjectPtr mPRTHandle;
};

using PRTContextUPtr = std::unique_ptr<PRTContext>;