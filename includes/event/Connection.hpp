#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Common.hpp"
#include "Core.hpp"
#include "IEventHandler.hpp"

#define RECV_BUFFER_SIZE 4096
#define SEND_BUFFER_SIZE 4096

class Connection : public IEventHandler {
public:
  Connection(int socket);
  ~Connection();
	virtual void HandleReadEvent();
	virtual void HandleWriteEvent();
  // virtual void HandleSignalEvent();
  virtual void HandleTimerEvent();
private:
  int mSocket;
  std::vector<char> mRecvBuffer;
  std::vector<char> mSendBuffer;
};

#endif