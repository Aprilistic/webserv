#include "WebServer.hpp"
#include "ServerConfig.hpp"

WebServer::WebServer(const std::string &path)
	: mKqueue(kqueue())
	, mConfigTree(Config::makeConfigTree(path))
	, mServerConfigList(Config::makeServerConfigList(this, mConfigTree))
{
	if (mKqueue == -1)
	{
		// error
	}
}

// void WebServer::EventMonitoring(void)
// {
// 	int newEvent = kevent(mKqueue, NULL, 0, &mEventList[0], mEventList.size(), NULL);

// 	if (newEvent < 0 && errno == EINTR)
// 	{
// 		return ;
// 	}

// 	for (int index = 0; index < newEvent; index++)
// 	{
// 		struct kevent currentEvent = mEventList[index];
// 		EventHandler(currentEvent);
// 	}
// }

// void WebServer::EventHandler(struct kevent& currentEvent)
// {
// 	ServerConfig *serverConfig = static_cast<ServerConfig *>(currentEvent.udata);

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
//           if ((clientSocket = accept(serverConfig->mSocket, NULL, NULL)) == -1) {
//             // error
//           }
//           fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
//           EV_SET(&events[0], clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL); // option 확인 필요 client 생성할지 고민
//           EV_SET(&events[1], clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL); // option 확인 필요 client 생성할지 고민
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

int WebServer::GetKqueue(void) const {
	return mKqueue;
}

WebServer::~WebServer(void) {}
