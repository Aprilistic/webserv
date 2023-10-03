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

  mServerConfigList = Config::makeServerConfigList(this, Common::mConfigTree);
  if (mServerConfigList.empty()) {
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

//   if (!mServerConfigList.empty()) {
//     for (std::vector<ServerConfig *>::iterator it = mServerConfigList.begin();
//          it != mServerConfigList.end(); ++it) {
//       	delete *it;
//     }
//   } else {
//     safeExit = 0;
//   }
   if (!mServerConfigList.empty()) {
        for (std::map<int, ServerConfig*>::iterator it = mServerConfigList.begin();
             it != mServerConfigList.end(); ++it) {
            delete it->second;
        }
        mServerConfigList.clear();
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
	if (mServerConfigList.count(currentEvent.ident) == true)
	{
		onServerRead(currentEvent.ident);
	}
	else if (mClientList.count(currentEvent.ident) == true)
	{
		onClientRead(currentEvent.ident);
	}
}

void WebServer::onServerRead(int ident)
{
	Client * client = new Client;
	struct kevent events[2];

	if ((client->mSocket = accept(mServerConfigList[ident]->mSocket, NULL, NULL)) == -1)
	{
		delete client;
		// error
	}
	fcntl(client->mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	EV_SET(&events[0], client->mSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, client);
	EV_SET(&events[1], client->mSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE,0, 0, client);
	kevent(Common::mKqueue, events, 2, NULL, 0, NULL);
	mClientList[client->mSocket] = client;
}

void WebServer::onClientRead(int ident)
{
	std::map<int, Client*>::iterator iter = mClientList.find(ident);
	if (iter == mClientList.end()) 
	{
		return;
	}
	Client* client = iter->second;
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
	if (mClientList.count(currentEvent.ident) == true)
	{
		onClientWrite(currentEvent.ident);
	}
}

void WebServer::onServerWrite(int ident)
{
}

void WebServer::onClientWrite(int ident)
{
    std::map<int, Client*>::iterator iter = mClientList.find(ident);
    if (iter == mClientList.end()) 
    {
        return;
    }
    Client* client = iter->second;
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
// 	if ((clientSocket = accept(serverConfig->mSocket, NULL, NULL)) ==
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
// 	ServerConfig *serverConfig = static_cast<ServerConfig
// *>(currentEvent.udata);

// 	if (currentEvent.flags & EV_ERROR)
// 	{
// 		// error
// 	}

// 	switch (currentEvent.filter)
// 	{
// 		case EVFILT_READ:
//         if (currentEvent.ident == serverConfig->mSocket) {
//           int clientSocket;
//           struct kevent events[2];
//           if ((clientSocket = accept(serverConfig->mSocket, NULL, NULL)) ==
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
//         serverConfig->ReadHandler(currentEvent.ident);
//         break;
// 		case EVFILT_WRITE:
// 			serverConfig->WriteHandler();
// 			break;
// 		case EVFILT_TIMER:
// 			serverConfig->TimerHandler();
// 			break;
// 		case EVFILT_SIGNAL:
// 			serverConfig->SignalHandler();
// 			break;
// 		default:
// 			break;
// 	}
// }
