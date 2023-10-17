#include "Http.hpp"

Http::Http() {}

Http::~Http() {}

eStatusCode Http::parseRequest(const std::vector<char> &buffer) {

  std::string tmp(buffer.begin(), buffer.end());
  mBuffer += tmp;

  RequestParser parser;
  eStatusCode result =
      parser.parse(mRequest, mBuffer.c_str(), mBuffer.c_str() + mBuffer.size());

  if (result == ParsingIncompleted) {
    return ParsingIncompleted;
  } else if (result == ParsingCompleted) {
    mBuffer = parser.getRemainingBuffer();
    return ParsingCompleted;
  } else {
    return CLIENT_ERROR_BAD_REQUEST;
  }
}

void Http::ErrorResponse(int port, eStatusCode errorStatus)
{
	Node* location = Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

	std::string errorPage = "error_page";
	std::vector<std::string> configErrorPageValues = location->FindValue(location, errorPage);

	if (configErrorPageValues.size() > 1)
	{
		for (std::vector<std::string>::iterator it = configErrorPageValues.begin(); it != configErrorPageValues.end() - 1; it++)
		{
			int errorCode;
			std::stringstream ss(*it);
			std::string errorPagePath;
            if (ss >> errorCode && errorCode == errorStatus)
            {
            	errorPagePath = configErrorPageValues.back();
				// std::cout << errorPagePath << std::endl;
                // errorPagePath response
				return ;
            }
		}
	}
	// default error page response
}

// std::vector<char>& Http::parseResponse(Response response)
// {
// }

void Http::resetRequest() { mRequest = Request(); }

void Http::resetResponse() { mResponse = Response(); }

Request &Http::getRequest() { return mRequest; }
