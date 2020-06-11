#pragma once

#include "PythonLogHandler.h"
#include "utils.h"

#include "prt/LogLevel.h"

/**
 * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
 */
struct PRTContext {
	static std::shared_ptr<PRTContext> get();
	static void shutdown();

	PRTContext(prt::LogLevel minimalLogLevel);
	~PRTContext();

	PythonLogHandler mLogHandler;
	pcu::ObjectPtr mPRTHandle;
};
