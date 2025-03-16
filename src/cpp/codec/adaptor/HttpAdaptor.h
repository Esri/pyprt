#pragma once

#include "prtx/EncoderInfoBuilder.h"
#include "prtx/Singleton.h"
#include "prtx/StreamAdaptor.h"
#include "prtx/StreamAdaptorFactory.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

class HttpAdaptor : public prtx::StreamAdaptor {
public:
	static const std::wstring ID;
	static const std::wstring NAME;
	static const std::wstring DESCRIPTION;

	[[nodiscard]] std::istream* createStream(prtx::URIPtr uri) const override;
	void destroyStream(std::istream* stream) const override;

private:
	mutable std::unique_ptr<Poco::Net::HTTPClientSession> mPocoSession;
	mutable std::unique_ptr<Poco::Net::HTTPRequest> mPocoRequest;
	mutable std::unique_ptr<Poco::Net::HTTPResponse> mPocoResponse;
};

class HttpAdaptorFactory : public prtx::StreamAdaptorFactory, public prtx::Singleton<HttpAdaptorFactory> {
public:
	static HttpAdaptorFactory* createInstance();

	[[nodiscard]] const std::wstring& getID() const override {
		return HttpAdaptor::ID;
	}

	[[nodiscard]] const std::wstring& getName() const override {
		return HttpAdaptor::NAME;
	}

	[[nodiscard]] const std::wstring& getDescription() const override {
		return HttpAdaptor::DESCRIPTION;
	}

	[[nodiscard]] bool canHandleURI(prtx::URIPtr uri) const override;

	[[nodiscard]] HttpAdaptor* create() const override;
};
