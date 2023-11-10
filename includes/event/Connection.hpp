#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Common.hpp"
#include "Core.hpp"
#include "Http.hpp"
#include "IEventHandler.hpp"

#define RECV_BUFFER_SIZE 16384
#define SEND_BUFFER_SIZE 16384

class Connection : public IEventHandler {
public:
  Connection(int socket, int port);
  ~Connection();
  virtual void EventHandler(struct kevent &currentEvent);

private:
  eStatusCode readFromSocket();
  void readHandler();
  void writeHandler();
  void disconnect();
private:
  int mSocket;
  int mPort;
  bool mKeepAlive;
  int mRemainingRequest;
  Http mHttp;
  std::vector<char> mRecvBuffer;
  std::string mSendBuffer;
};

#endif