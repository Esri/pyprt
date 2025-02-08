#pragma once

#include <utility>

#include "prtx/Extension.h"
#include "prtx/StreamAdaptor.h"
#include "prtx/StreamAdaptorFactory.h"

#include "pybind11/pybind11.h"

#include <istream>
#include <streambuf>

class PyStreamBuf : public std::streambuf {
public:
	explicit PyStreamBuf(pybind11::object pyFile) : mPyFile(pyFile) {}

protected:
	int underflow() override {
		if (gptr() && (gptr() < egptr())) // Buffer not exhausted
			return traits_type::to_int_type(*gptr());

		pybind11::gil_scoped_acquire acquire;
		pybind11::object data = mPyFile.attr("read")(4096);
		buffer = data.cast<std::string>();

		if (buffer.empty())
			return traits_type::eof();

		setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
		return traits_type::to_int_type(*gptr());
	}

private:
	pybind11::object mPyFile;
	std::string buffer;
};

// trampoline
class PyStream : public std::istream {
public:
	explicit PyStream(pybind11::object pyFile) : std::istream(&mStreamBuf), mStreamBuf(pyFile) {}

private:
	PyStreamBuf mStreamBuf;
};

using PyStreamPtr = std::shared_ptr<PyStream>;


class StreamWrapper : public std::istream {
public:
	explicit StreamWrapper(PyStreamPtr delegate) : mDelegate(delegate) {}

private:
	PyStreamPtr mDelegate;
};

/**
 * this is the Python trampoline, it calls the Python class
 */
class PyAdaptor : public prtx::StreamAdaptor {
public:
	[[nodiscard]] std::istream* createStream(prtx::URIPtr uri) const override;
	void destroyStream(std::istream* stream) const override;
};

using PyAdaptorPtr = std::shared_ptr<PyAdaptor>;

/**
 * this is the actual adaptor as seen (and managed) by PRT
 * it forwards calls to the trampoline
 */
class AdaptorWrapper : public prtx::StreamAdaptor {
public:
	explicit AdaptorWrapper(PyAdaptorPtr delegate) : mDelegate(std::move(delegate)) {}

	[[nodiscard]] std::istream* createStream(prtx::URIPtr uri) const override {
		return mDelegate->createStream(uri);
	}

	void destroyStream(std::istream* stream) const override {
		return mDelegate->destroyStream(stream);
	}

private:
	PyAdaptorPtr mDelegate;
};

/**
 * this is the Python trampoline, it calls the Python class
 */
class PyAdaptorFactory : public prtx::StreamAdaptorFactory {
public:
	PyAdaptorFactory() = default;

	[[nodiscard]] prtx::Extension* create() const override;
	[[nodiscard]] float getMerit() const override;
	[[nodiscard]] std::wstring const& getID() const override;
	[[nodiscard]] std::wstring const& getName() const override;
	[[nodiscard]] std::wstring const& getDescription() const override;
	[[nodiscard]] bool canHandleURI(prtx::URIPtr uri) const override;
};

using PyAdaptorFactoryPtr = std::shared_ptr<PyAdaptorFactory>;

/**
 * this is the actual adaptor factory as seen (and managed) by PRT
 * it forwards calls to the trampoline
 */
class AdaptorFactoryWrapper : public prtx::StreamAdaptorFactory {
public:
	explicit AdaptorFactoryWrapper(PyAdaptorFactoryPtr delegate) : mDelegate(std::move(delegate)) {}

	[[nodiscard]] prtx::Extension* create() const override {
		return mDelegate->create();
	}

	[[nodiscard]] float getMerit() const override {
		return mDelegate->getMerit();
	}

	[[nodiscard]] std::wstring const& getID() const override {
		return mDelegate->getID();
	}

	[[nodiscard]] std::wstring const& getName() const override {
		return mDelegate->getName();
	}

	[[nodiscard]] std::wstring const& getDescription() const override {
		return mDelegate->getDescription();
	}

	[[nodiscard]] bool canHandleURI(prtx::URIPtr uri) const override {
		return mDelegate->canHandleURI(uri);
	}

private:
	PyAdaptorFactoryPtr mDelegate;
};