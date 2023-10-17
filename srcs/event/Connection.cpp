#include "Connection.hpp"

Connection::Connection(int socket, int port) : mSocket(socket), mPort(port) {
  struct kevent events[2];

  mRecvBuffer.reserve(RECV_BUFFER_SIZE);
  mSendBuffer.reserve(SEND_BUFFER_SIZE);

  fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, this);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
}

Connection::~Connection() { close(mSocket); }


// void Connection::ErrorResponse(eStatusCode errorStatus)
// {
// 	Node* location = Common::mConfigMap->GetConfigNode(mPort, mHttp.getRequest().mHost, mHttp.getRequest().mUri);

// 	std::string errorPage = "error_page";
// 	std::vector<std::string> configErrorPageValues = location->FindValue(location, errorPage);

// 	if (configErrorPageValues.size() > 1)
// 	{
// 		for (std::vector<std::string>::iterator it = configErrorPageValues.begin(); it != configErrorPageValues.end() - 1; it++)
// 		{
// 			int errorCode;
// 			std::stringstream ss(*it);
// 			std::string errorPagePath;
//             if (ss >> errorCode && errorCode == errorStatus)
//             {
//             	errorPagePath = configErrorPageValues.back();
//                 // errorPagePath response
// 				return ;
//             }
// 		}
// 	}
// 	// default error page response
// }


void Connection::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    // error
  }
  switch (currentEvent.filter) {
  case EVFILT_READ:
    readHandler();
    break;
  case EVFILT_WRITE:
    writeHandler();
    break;
  case EVFILT_TIMER:
    timerHandler();
    break;
  case EVFILT_SIGNAL:
    signalHandler();
    break;
  default:
    assert("Connection::EventHandler: default" == 0);
    break;
  }
}

#include "Node.hpp"

void Connection::readHandler() {
  mRecvBuffer.clear();
  ssize_t bytesRead;
  char tmp[RECV_BUFFER_SIZE];
  bytesRead = recv(mSocket, tmp, RECV_BUFFER_SIZE, 0);
  mRecvBuffer.insert(mRecvBuffer.end(), tmp, tmp + bytesRead);

  // std::cout << "bytesRead: " << bytesRead << std::endl;
  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      // error
    }
    // disconnection();
    return;
  }

  eStatusCode parseStatus = mHttp.parseRequest(mRecvBuffer);

  if (parseStatus == ParsingIncompleted) {
	return ;
  }
  
  switch (parseStatus)
  {
	case (ParsingCompleted):
		break ;
	case (ParsingIncompleted):
		return ;
	default:
		return (mHttp.ErrorResponse(mPort, parseStatus));
  }
  


  // 포트가 같은데 둘 다 이름이 없는 경우 localhost로 접근할 때,
  // default_server로 안 가는 문제
  Node *test = Common::mConfigMap->GetConfigNode(mPort, mHttp.getRequest().mHost, mHttp.getRequest().mUri);

  std::string str = "location";
  std::cout << test->FindValue(test, str)[0] << std::endl;
  
//   mHttp.ErrorResponse(mPort, CLIENT_ERROR_NOT_FOUND);
  // std::string str1 = "client_max_body_size";
  // std::cout << test->FindValue(test, str1)[0] << std::endl;

  Node *location = Common::mConfigMap->GetConfigNode(mPort, "", "/");

  // client_max_bidy_size
  std::string cmbs = "client_max_body_size";

  if (location->FindValue(location, cmbs).size()) {
    int clientMaxBodySize = std::atoi(
        location->FindValue(location, cmbs)[0].c_str()); // overflow, k,M,G

    std::multimap<std::string, std::string>::iterator it;
    it = mHttp.getRequest().mHeaders.find("Content-Length");

    if (it != mHttp.getRequest().mHeaders.end()) {
      int contentLength = std::atoi(it->second.c_str());
      if (contentLength > clientMaxBodySize) {
        // CLIENT_ERROR_CONTENT_TOO_LARGE 413 error
      }
    }
  }

  // return
  int retValue;
  std::string retPath;
  std::string ret = "return";

  if (mHttp.getRequest().mUri == "/") {
    std::vector<std::string> Topvalue;
    Topvalue =
        location->FindTopValue(location, ret, std::vector<std::string>());
    if (Topvalue.size()) {
      retValue = std::atoi(Topvalue[0].c_str());
      retPath = Topvalue[1];
    }
  } else {
    std::vector<std::string> downValue;
    downValue = location->FindValue(location, ret);
    if (downValue.size()) {
      retValue = std::atoi(downValue[0].c_str());
      retPath = downValue[1];
    }
  }

  // limit_except

  std::string limitExcept = "limit_except";
  std::vector<std::string> limitExceptValue =
      location->FindValue(location, limitExcept); // 초기화가 필요합니다.
  if (limitExceptValue.size()) {
    if (std::find(limitExceptValue.begin(), limitExceptValue.end(),
                  mHttp.getRequest().mMethod) == limitExceptValue.end()) {
      // CLIENT_ERROR_METHOD_NOT_ALLOWED 405 error
    }
  }

  //
  std::multimap<std::string, std::string>::iterator it;
  it = mHttp.getRequest().mHeaders.find("Host");

  if (it == mHttp.getRequest().mHeaders.end()) {
    // CLIENT_ERROR_BAD_REQUEST 400 error
  }

  // std::string hostName = it->second;
  // Node *location = Common::mConfigMap->GetConfigNode(mPort, hostName, mHttpParser.getRequest().mUri);

  // Router router;

  // IRequestHandler* handler =  router.Routing(mHttp.getRequest());

  // Response response = handler->handle(mHttp.getRequest());

  // mSendBuffer = mHttp.parseResponse(response);
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, &mSendBuffer[0], mSendBuffer.size(), 0);

  if (bytesSent <= 0) {
    if (bytesSent < 0) {
      // error
    }
    return;
  }
  mHttp.resetResponse();
}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}
