#include "adaptor/HttpAdaptor.h"

#include "utils.h"

#include "Poco/Net/HTTPMessage.h"
#include <istream>

const std::wstring HttpAdaptor::ID = L"com.esri.pyprt.HttpAdaptor";
const std::wstring HttpAdaptor::NAME = L"PRT HTTP/S Adaptor";
const std::wstring HttpAdaptor::DESCRIPTION = L"Opens data streams from HTTP/S URLs.";

// TODO: support multiple calls to createStream with different uris concurrently
std::istream* HttpAdaptor::createStream(prtx::URIPtr uri) const {
	const std::string host = pcu::toUTF8FromUTF16(uri->getHost());
	const std::string path = pcu::toUTF8FromUTF16(uri->getPath());

	mPocoSession = std::make_unique<Poco::Net::HTTPClientSession>(host);
	mPocoRequest = std::make_unique<Poco::Net::HTTPRequest>(Poco::Net::HTTPRequest::HTTP_GET, path,
	                                                        Poco::Net::HTTPMessage::HTTP_1_1);
	mPocoResponse = std::make_unique<Poco::Net::HTTPResponse>();

	mPocoSession->sendRequest(*mPocoRequest);
	return &mPocoSession->receiveResponse(*mPocoResponse);
}

void HttpAdaptor::destroyStream(std::istream*) const {
	mPocoResponse.reset();
	mPocoRequest.reset();
	mPocoSession.reset();
}

[[nodiscard]] HttpAdaptorFactory* HttpAdaptorFactory::createInstance() {
	return new HttpAdaptorFactory();
}

bool HttpAdaptorFactory::canHandleURI(prtx::URIPtr uri) const {
	return (uri->getScheme().find(L"http") == 0);
}

HttpAdaptor* HttpAdaptorFactory::create() const {
	return new HttpAdaptor();
}
