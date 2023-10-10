#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Common.hpp"
#include "Core.hpp"
#include "HttpParser.hpp"
#include "IEventHandler.hpp"

#define RECV_BUFFER_SIZE 4096
#define SEND_BUFFER_SIZE 4096

class Connection : public IEventHandler {
public:
  Connection(int socket);
  ~Connection();
  virtual void EventHandler(struct kevent &currentEvent);

private:
  void readHandler();
  void writeHandler();
  void timerHandler();
  void signalHandler();

private:
  int mSocket;
  HttpParser mHttpParser;
  std::vector<char> mRecvBuffer;
  std::vector<char> mSendBuffer;
};

#endif