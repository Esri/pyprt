#include "PRTContext.h"

#include <array>
#include <string>

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

PRTContext::operator bool() const {
	return (bool)mPRTHandle;
}
