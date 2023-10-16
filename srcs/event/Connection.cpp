#include "Connection.hpp"

Connection::Connection(int socket) : mSocket(socket) {
  struct kevent events[2];

  mRecvBuffer.reserve(RECV_BUFFER_SIZE);
  mSendBuffer.reserve(SEND_BUFFER_SIZE);

  fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, this);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
}

Connection::~Connection() { close(mSocket); }

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
  bytesRead = recv(mSocket, &mRecvBuffer[0], RECV_BUFFER_SIZE, 0);

  std::cout << bytesRead << std::endl;
  if (bytesRead <= 0) {
    if (bytesRead < 0) {
      // error
    }
    // disconnection();
    return;
  }

  // if (mHttpParser.parseRequest(mRecvBuffer) == ParsingIncompleted) {
  //   return;
  // }
  // std::string string = "GET / HTTP/1.1\r\n"
  //                      "Host: localhost:4242\r\n"
  //                      "User-Agent: curl/7.64.1\r\n"
  //                      "Accept: */*\r\n\r\n";

  // std::vector<char> wow;
  // wow = std::vector<char>(string.begin(), string.end());
  // std::cout << "this: " << mHttpParser.parseRequest(mRecvBuffer) << std::endl;

  if (mHttpParser.parseRequest(mRecvBuffer) == ParsingIncompleted) {
    return;
  }
  // 포트가 같은데 둘 다 이름이 없는 경우 localhost로 접근할 때,
  // default_server로 안 가는 문제
  Node *test = Common::mConfigMap->GetConfigNode(4242, "test", "/");

  std::string str = "alias";
  std::cout << test->FindValue(test, str)[0] << std::endl;

  std::string str1 = "client_max_body_size";
  std::cout << test->FindValue(test, str1)[0] << std::endl;

  Node *location = Common::mConfigMap->GetConfigNode(4242, "", "/");

  // client_max_bidy_size
  std::string cmbs = "client_max_body_size";

  if (location->FindValue(location, cmbs).size()) {
    int clientMaxBodySize = std::atoi(
        location->FindValue(location, cmbs)[0].c_str()); // overflow, k,M,G

    std::multimap<std::string, std::string>::iterator it;
    it = mHttpParser.getRequest().mHeaders.find("Content-Length");

    if (it != mHttpParser.getRequest().mHeaders.end()) {
      int contentLength = std::atoi(it->second.c_str());
      if (contentLength > clientMaxBodySize) {
        // CLIENT_ERROR_CONTENT_TOO_LARGE 413 error
      }
    }
  }

  std::cout << location->FindValue(test, str1)[0] << std::endl;

  // return
  int retValue;
  std::string retPath;
  std::string ret = "return";

  if (mHttpParser.getRequest().mUri == "/") {
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
                  mHttpParser.getRequest().mMethod) == limitExceptValue.end()) {
      // CLIENT_ERROR_METHOD_NOT_ALLOWED 405 error
    }
  }

  // Router router;

  // IRequestHandler* handler =  router.Routing(mHttpParser.getRequest());

  // Response response = handler->handle(mHttpParser.getRequest());

  // mSendBuffer = mHttpParser.parseResponse(response);
}

void Connection::writeHandler() {
  ssize_t bytesSent = send(mSocket, &mSendBuffer[0], mSendBuffer.size(), 0);

  if (bytesSent <= 0) {
    if (bytesSent < 0) {
      // error
    }
    return;
  }
  mHttpParser.resetResponse();
}

void Connection::timerHandler() {
  // error
}

void Connection::signalHandler() {
  // error
}
