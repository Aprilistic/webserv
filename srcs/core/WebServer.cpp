#include "WebServer.hpp"
#include "Config.hpp"

WebServer::WebServer(const std::string &path)
		: mGood(true)
{
  Config::makeConfigTree(path);
  if (Common::mConfigTree == NULL) {
		mGood = false;
    return;
  }

  mServerList = Config::makeServerList(this, Common::mConfigTree);
  if (mServerList.empty()) {
		mGood = false;
    return;
  }

	Common::mKqueue = kqueue();
  if (Common::mKqueue == -1) {
		mGood = false;
    return;
  }
}

bool WebServer::IsGood(void) const{
		return (mGood);
}

WebServer::~WebServer(void) {
  int safeExit = 1;

  if (Common::mKqueue != -1) {
    close(Common::mKqueue);
  } else {
    safeExit = 0;
  }

   if (!mServerList.empty()) {
        for (std::map<int, Server*>::iterator it = mServerList.begin();
             it != mServerList.end(); ++it) {
            delete it->second;
        }
        mServerList.clear();
    } else {
        safeExit = 0;
    }

  if (Common::mConfigTree) {
    delete (Common::mConfigTree);
  } else {
    safeExit = 0;
  }

  if (safeExit == 0) {
    exit(1);
  }
}

void WebServer::Run(void)
{
	if (!IsGood())
	{
		return;
	}
	eventMonitoring();
}

void WebServer::eventMonitoring(void)
{
	while (true)
	{
		int newEvent = kevent(Common::mKqueue, NULL, 0, &mEventList[0], mEventList.size(), NULL);

		if (newEvent < 0 && errno == EINTR)
		{
			return ;
		}

		for (int index = 0; index < newEvent; index++)
		{
			struct kevent currentEvent = mEventList[index];
			eventHandler(currentEvent);
		}
	}
}

void WebServer::eventHandler(struct kevent& currentEvent)
{
	if (currentEvent.flags & EV_ERROR)
	{
		// error
	}
	switch (currentEvent.filter)
	{
		case EVFILT_READ:
			handleReadEvent(currentEvent);
        	break;
		case EVFILT_WRITE:
			handleWriteEvent(currentEvent);
			break;
		case EVFILT_TIMER:
			// handleTimerEvent(currentEvent);
			break;
		case EVFILT_SIGNAL:
			// handleSignalEvent(currentEvent);
			break;
		default:
			break;
	}
}

void WebServer::handleReadEvent(struct kevent& currentEvent)
{
	if (mServerList.count(currentEvent.ident) == true)
	{
		onServerRead(currentEvent.ident);
	}
	else if (mConnectionList.count(currentEvent.ident) == true)
	{
		onConnectionRead(currentEvent.ident);
	}
}

void WebServer::onServerRead(int ident)
{
	Connection * client = new Connection;
	struct kevent events[2];

	if ((client->mSocket = accept(mServerList[ident]->mSocket, NULL, NULL)) == -1)
	{
		delete client;
		// error
	}
	fcntl(client->mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	EV_SET(&events[0], client->mSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, client);
	EV_SET(&events[1], client->mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE,0, 0, client);
	kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
	mConnectionList[client->mSocket] = client;
}

void WebServer::onConnectionRead(int ident)
{
	std::map<int, Connection*>::iterator iter = mConnectionList.find(ident);
	if (iter == mConnectionList.end()) 
	{
		return;
	}
	Connection* client = iter->second;
	char tempBuffer[1024];
	ssize_t bytesRead;

	bytesRead = recv(client->mSocket, tempBuffer, sizeof(tempBuffer), 0);

	if (bytesRead <= 0)
	{
		if (bytesRead < 0)
		{
			// error
		}
		// disconnection();
		return ;
	}

	client->mRecvBuffer.insert(client->mRecvBuffer.end(), tempBuffer, tempBuffer + bytesRead);
}


void WebServer::handleWriteEvent(struct kevent& currentEvent)
{
	if (mConnectionList.count(currentEvent.ident) == true)
	{
		onConnectionWrite(currentEvent.ident);
	}
}

void WebServer::onServerWrite(int ident)
{
}

void WebServer::onConnectionWrite(int ident)
{
    std::map<int, Connection*>::iterator iter = mConnectionList.find(ident);
    if (iter == mConnectionList.end()) 
    {
        return;
    }
    Connection* client = iter->second;
    if (client->mSendBuffer.empty()) 
    {
        return;
    }

    ssize_t bytesSent = send(client->mSocket, &client->mSendBuffer[0], client->mSendBuffer.size(), 0);
    
    if (bytesSent <= 0) 
    {
        if (bytesSent < 0) 
        {
			//error
        }
        return;
    }

    client->mSendBuffer.erase(client->mSendBuffer.begin(), client->mSendBuffer.begin() + bytesSent);
}


// void ServerHandler::ReadHandler()
// {
// 	int clientSocket;
// 	struct kevent events[2];
// 	if ((clientSocket = accept(Server->mSocket, NULL, NULL)) ==
// 	-1) {
// 	// error
// 	}
// 	fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
// 	EV_SET(&events[0], clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE,
// 	0, 0, NULL); // option 확인 필요 client 생성할지 고민
// 	EV_SET(&events[1], clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE,
// 	0, 0, NULL); // option 확인 필요 client 생성할지 고민
// 	kevent(mKqueue, events, 2, NULL, 0, NULL);
// }

// void WebServer::EventHandler(struct kevent& currentEvent)
// {
// 	Server *Server = static_cast<Server
// *>(currentEvent.udata);

// 	if (currentEvent.flags & EV_ERROR)
// 	{
// 		// error
// 	}

// 	switch (currentEvent.filter)
// 	{
// 		case EVFILT_READ:
//         if (currentEvent.ident == Server->mSocket) {
//           int clientSocket;
//           struct kevent events[2];
//           if ((clientSocket = accept(Server->mSocket, NULL, NULL)) ==
//           -1) {
//             // error
//           }
//           fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
//           EV_SET(&events[0], clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE,
//           0, 0, NULL); // option 확인 필요 client 생성할지 고민
//           EV_SET(&events[1], clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE,
//           0, 0, NULL); // option 확인 필요 client 생성할지 고민
//           kevent(mKqueue, events, 2, NULL, 0, NULL);
//         }
//         Server->ReadHandler(currentEvent.ident);
//         break;
// 		case EVFILT_WRITE:
// 			Server->WriteHandler();
// 			break;
// 		case EVFILT_TIMER:
// 			Server->TimerHandler();
// 			break;
// 		case EVFILT_SIGNAL:
// 			Server->SignalHandler();
// 			break;
// 		default:
// 			break;
// 	}
// }
