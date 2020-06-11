#pragma once

#include "utils.h"
#include "wrap.h"

#include "prt/LogLevel.h"

/**
 * Helper struct to manage PRT lifetime (e.g. the prt::init() call)
 */
struct PRTContext {
	PRTContext(prt::LogLevel minimalLogLevel);
	~PRTContext();

	explicit operator bool() const;

	PythonLogHandler mLogHandler;
	pcu::ObjectPtr mPRTHandle;
};
