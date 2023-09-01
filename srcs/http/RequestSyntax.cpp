#include "RequestSyntax.hpp"

RequestSyntax::RequestSyntax() : mState(SUCCESSFUL_OK) {}

RequestSyntax::~RequestSyntax() {}

eSyntaxResult RequestSyntax::syntax(Request &request) {
  if (method(request) != SUCCESSFUL_OK) {
    return mState;
  }
  if (requestTarget(request) != SUCCESSFUL_OK) {
    return mState;
  }
  if (httpVersion(request) != SUCCESSFUL_OK) {
    return mState;
  }
  if (headerField(request) != SUCCESSFUL_OK) {
    return mState;
  }
  if (messageBody(request) != SUCCESSFUL_OK) {
    return mState;
  }
  return SUCCESSFUL_OK;
}

eSyntaxResult RequestSyntax::method(Request &request) {
  if (request.mMethod == "GET" || request.mMethod == "POST" ||
      request.mMethod == "DELETE") {
    return SUCCESSFUL_OK;
  }
  else {
	mState = CLIENT_ERROR_METHOD_NOT_ALLOWED;
  	return (CLIENT_ERROR_METHOD_NOT_ALLOWED);
  }
}

eSyntaxResult RequestSyntax::requestTarget(Request &request) {
  if (request.mUri.empty()) {
	mState = CLIENT_ERROR_BAD_REQUEST;
    return mState;
  }
  else {
  return SUCCESSFUL_OK;
  }
}

eSyntaxResult RequestSyntax::httpVersion(Request &request) {
  if (request.mVersionMajor != 1 || request.mVersionMinor != 1)
    return mState;
  return SUCCESSFUL_OK;
}

eSyntaxResult RequestSyntax::headerField(Request &request) {
  if (request.mHeaders.empty())
    return mState;
  for (std::vector<Request::HeaderItem>::iterator it = request.mHeaders.begin();
       it != request.mHeaders.end(); ++it) {
  }
  return SUCCESSFUL_OK;
}

eSyntaxResult RequestSyntax::messageBody(Request &request) {
  return SUCCESSFUL_OK;
}