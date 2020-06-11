#include "PythonLogHandler.h"
#include "wrap.h"

#include <pybind11/pybind11.h>

#include <sstream>

void PythonLogHandler::handleLogEvent(const wchar_t* msg, prt::LogLevel /*level*/) {
	pybind11::print(L"[PRT]", msg);
}

const prt::LogLevel* PythonLogHandler::getLevels(size_t* count) {
	*count = prt::LogHandler::ALL_COUNT;
	return prt::LogHandler::ALL;
}

void PythonLogHandler::getFormat(bool* dateTime, bool* level) {
	*dateTime = true;
	*level = true;
}

char* PythonLogHandler::toXML(char* result, size_t* resultSize, prt::Status* stat) const {
	std::ostringstream out;
	out << *this;
	copyToCStr(out.str(), result, *resultSize);
	if (stat)
		*stat = prt::STATUS_OK;
	return result;
}
