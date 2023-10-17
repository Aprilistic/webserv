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
				std::cout << errorPagePath << std::endl;
                // errorPagePath response
				return ;
            }
		}
	}
	// default error page response
}

  bool Http::CheckRedirect(int port)
  {
	Node* location = Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

	int redirectCode;
	std::string redirectPath;
	std::string redirect = "return";

	if (mRequest.mUri == "/") {
		std::vector<std::string> Topvalue;
		Topvalue =
			location->FindTopValue(location, redirect, std::vector<std::string>());
		if (Topvalue.size()) {
		redirectCode = std::atoi(Topvalue[0].c_str());
		redirectPath = Topvalue[1];
		}
		else
			return (false);
	} else {
		std::vector<std::string> downValue;
		downValue = location->FindValue(location, redirect);
		if (downValue.size()) {
		redirectCode = std::atoi(downValue[0].c_str());
		redirectPath = downValue[1];
		}
		else
			return (false);
	}
	return (true);
  }
  
  bool Http::checkClientMaxBodySize(int port)
  {
	Node* location = Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

	std::string clientMaxBodySize = "client_max_body_size";
	std::vector<std::string> clientMaxBodySizeValues = location->FindValue(location, clientMaxBodySize);


	if (clientMaxBodySizeValues.size()) {
		int valueSize = std::atoi(
			clientMaxBodySizeValues[0].c_str()); // overflow, k,M,G check

		std::multimap<std::string, std::string>::iterator it;
		it = mRequest.mHeaders.find("Content-Length");

		if (it != mRequest.mHeaders.end()) {
		int contentLength = std::atoi(it->second.c_str());
			if (contentLength > valueSize) {
				return (false);
			}
		}
	}
	return (true);
  }

  bool Http::CheckLimitExcept(int port)
  {
	Node* location = Common::mConfigMap->GetConfigNode(port, mRequest.mHost, mRequest.mUri);

	std::string limitExcept = "limit_except";
	std::vector<std::string> limitExceptValue =
		location->FindValue(location, limitExcept); // 초기화가 필요합니다.
		if (limitExceptValue.size()) {
			if (std::find(limitExceptValue.begin(), limitExceptValue.end(),
						mRequest.mMethod) == limitExceptValue.end()) {
				return (false);
			}
		}
	return (true);
  }

// std::vector<char>& Http::parseResponse(Response response)
// {
// }



void Http::resetRequest() { mRequest = Request(); }

void Http::resetResponse() { mResponse = Response(); }

Request &Http::getRequest() { return mRequest; }
Response &Http::getResponse () { return mResponse; }