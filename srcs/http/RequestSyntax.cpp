#include "RequestSyntax.hpp"

RequestSyntax::RequestSyntax() {}

RequestSyntax::~RequestSyntax() {}

eStatustCode RequestSyntax::checksyntax(Request &request) {
    typedef eStatustCode (RequestSyntax::*CheckFunction)(Request&);
    CheckFunction checks[] = {
        &RequestSyntax::method,
        &RequestSyntax::requestTarget,
        &RequestSyntax::httpVersion,
        &RequestSyntax::headerField,
        &RequestSyntax::messageBody
    };

    size_t numChecks = sizeof(checks) / sizeof(CheckFunction);
    for (size_t i = 0; i < numChecks; ++i) {
        eStatustCode result = (this->*checks[i])(request);
        if (result != SUCCESSFUL_OK) {
            return result;  
        }
    }

    return SUCCESSFUL_OK; 
}


eStatustCode RequestSyntax::method(Request &request) {
  if (request.mMethod == "GET" || request.mMethod == "POST" ||
      request.mMethod == "DELETE") {
    return SUCCESSFUL_OK;
  }
  else {
  	return (CLIENT_ERROR_METHOD_NOT_ALLOWED);
  }
}

eStatustCode RequestSyntax::requestTarget(Request &request) {
  if (request.mUri.empty()) {
    return (CLIENT_ERROR_BAD_REQUEST);
  }
  else {
  return SUCCESSFUL_OK;
  }
}

eStatustCode RequestSyntax::httpVersion(Request &request) {
  if (request.mVersionMajor != 1 || request.mVersionMinor != 1) {
    return CLIENT_ERROR_BAD_REQUEST;
  }
  return SUCCESSFUL_OK;
}

eStatustCode RequestSyntax::headerField(Request &request) {
  int hostCount = 0;

  for (std::vector<Request::HeaderItem>::const_iterator it = request.mHeaders.begin(); 
       it != request.mHeaders.end(); ++it) 
  {
    std::string upperName = it->name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);

    if (upperName == "HOST") {
      hostCount++;
      
      if (it->value.empty() || hostCount > 1) {
        return CLIENT_ERROR_BAD_REQUEST;
      }
    }
  }
  return SUCCESSFUL_OK;
}

eStatustCode RequestSyntax::messageBody(Request &request) {
  return SUCCESSFUL_OK;
}