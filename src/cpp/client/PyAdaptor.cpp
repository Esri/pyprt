#include "PyAdaptor.h"

#include <codecvt>
#include <istream>
#include <locale>
#include <string>

namespace {

std::wstring utf8_to_wstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}

template <typename T, typename R, typename... ARGS>
R callOverride(T const* self, std::string const& funcName, ARGS&&... args) {
	pybind11::function override = pybind11::get_override(self, funcName.c_str());
	if (!override)
		throw std::runtime_error("could not call " + funcName);
	return override(args...).template cast<R>();
}

// specialization for functions with wstring const& return type
template <typename... ARGS>
std::wstring const& callOverride(PyAdaptorFactory const* self, std::string const& funcName, std::wstring& storage,
                                 ARGS&&... args) {
	pybind11::function override = pybind11::get_override(self, funcName.c_str());
	if (!override)
		throw std::runtime_error("could not call " + funcName);
	auto utf8_str = override(args...).template cast<std::string>();
	storage = utf8_to_wstring(utf8_str);
	return storage;
}

} // namespace

std::istream* PyAdaptor::createStream(prtx::URIPtr uri) const {
	pybind11::gil_scoped_acquire gil;
	auto pyStream = callOverride<PyAdaptor, pybind11::object>(this, "create_stream", uri->string());
	return new StreamWrapper(pyStream.cast<std::shared_ptr<PyStream>>());
}

void PyAdaptor::destroyStream(std::istream* stream) const {
	pybind11::gil_scoped_acquire gil;
	delete stream;
}

prtx::Extension* PyAdaptorFactory::create() const {
	pybind11::gil_scoped_acquire gil;
	auto pyAdaptor = callOverride<PyAdaptorFactory, pybind11::object>(this, "create_adaptor");
	auto adaptor = pyAdaptor.cast<std::shared_ptr<PyAdaptor>>();
	return new AdaptorWrapper(adaptor); // ownership transferred to PRT
}

float PyAdaptorFactory::getMerit() const {
	pybind11::gil_scoped_acquire gil;
	return callOverride<PyAdaptorFactory, float>(this, "get_merit");
}

const std::wstring& PyAdaptorFactory::getID() const {
	static std::wstring storage;
	pybind11::gil_scoped_acquire gil;
	return callOverride(this, "get_id", storage);
}

const std::wstring& PyAdaptorFactory::getName() const {
	static std::wstring storage;
	pybind11::gil_scoped_acquire gil;
	return callOverride(this, "get_name", storage);
}

const std::wstring& PyAdaptorFactory::getDescription() const {
	static std::wstring storage;
	pybind11::gil_scoped_acquire gil;
	return callOverride(this, "get_description", storage);
}

bool PyAdaptorFactory::canHandleURI(prtx::URIPtr uri) const {
	pybind11::gil_scoped_acquire gil;
	return callOverride<PyAdaptorFactory, bool, std::string>(this, "can_handle_uri", uri->string());
}