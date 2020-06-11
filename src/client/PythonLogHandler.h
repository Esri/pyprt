#pragma once

#include "prt/LogHandler.h"

#include <ostream>

/**
 * custom console logger to redirect PRT log events into the python output
 */
class PythonLogHandler : public prt::LogHandler {
public:
	PythonLogHandler() = default;
	virtual ~PythonLogHandler() = default;

	void handleLogEvent(const wchar_t* msg, prt::LogLevel level) override;
	const prt::LogLevel* getLevels(size_t* count) override;
	void getFormat(bool* dateTime, bool* level) override;

	char* toXML(char* result, size_t* resultSize, prt::Status* stat = nullptr) const;
	friend std::ostream& operator<<(std::ostream& stream, const PythonLogHandler&) {
		stream << "<PythonLogHandler />";
		return stream;
	}
};
