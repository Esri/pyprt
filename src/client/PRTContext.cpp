/**
 * PyPRT - Python Bindings for the Procedural Runtime (PRT) of CityEngine
 *
 * Copyright (c) 2012-2023 Esri R&D Center Zurich
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

#include "PRTContext.h"
#include "utils.h"

#include <array>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>

namespace {

std::shared_ptr<PRTContext> prtCtx;
std::once_flag prtInitFlag;

} // namespace

std::shared_ptr<PRTContext> PRTContext::get() {
	std::call_once(prtInitFlag, []() { prtCtx = std::make_shared<PRTContext>(prt::LOG_WARNING); });
	return prtCtx;
}

void PRTContext::shutdown() {
	prtCtx.reset();
}

PRTContext::PRTContext(prt::LogLevel minimalLogLevel) {
	prt::addLogHandler(&mLogHandler);

	// setup path for PRT extension libraries
	const std::filesystem::path moduleRoot = pcu::getModuleDirectory().parent_path();
	const auto prtExtensionPath = moduleRoot / "lib";

	// initialize PRT with the path to its extension libraries, the default log
	// level
	const std::wstring wExtPath = prtExtensionPath.wstring();
	const std::array<const wchar_t*, 1> extPaths = {wExtPath.c_str()};
	mPRTHandle.reset(prt::init(extPaths.data(), extPaths.size(), minimalLogLevel));
}

PRTContext::~PRTContext() {
	// shutdown PRT
	mPRTHandle.reset();

	// remove loggers
	prt::removeLogHandler(&mLogHandler);
}
