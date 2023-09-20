#include "Master.hpp"
#include "Server.hpp"

// Master::Master(const std::string &path) {
//   Config config(path);
//   EventMonitor eventMonitor(config.GetConfigTree());
// }

Master::Master(const std::string &path)
{
	mKqueue = kqueue();

	if (mKqueue == -1)
	{
		// error
	}

	Config config(path);
	Node* httpNode = config.GetConfigTree()->mChildren[0];
	for (std::vector<Node *>::iterator serverNode = httpNode->mChildren.begin();
		serverNode != httpNode->mChildren.end(); ++serverNode)
		{
			mServerList.push_back(new Server(this, *serverNode));
		}
}

void Master::EventMonitoring(void)
{
	int newEvent = kevent(mKqueue, NULL, 0, &mEventList[0], mEventList.size(), NULL);

	if (newEvent < 0 && errno == EINTR)
	{
		return ;
	}

	for (int index = 0; index < newEvent; index++)
	{
		struct kevent currentEvent = mEventList[index];
		EventHandler(currentEvent);
	}
}

void Master::EventHandler(struct kevent& currentEvent)
{
	Server *server = static_cast<Server *>(currentEvent.udata);

	if (currentEvent.fflags & EV_ERROR)
	{
		// error
	}

	switch (currentEvent.filter)
	{
		case EVFILT_READ:
        if (currentEvent.ident == server->mSocket) {
          int clientSocket;
          struct kevent events[2];
          if ((clientSocket = accept(server->mSocket, NULL, NULL)) == -1) {
            // error
          }
          fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
          EV_SET(&events[0], clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL); // option 확인 필요 client 생성할지 고민
          EV_SET(&events[1], clientSocket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL); // option 확인 필요 client 생성할지 고민
          kevent(mKqueue, events, 2, NULL, 0, NULL);
        }
        server->ReadHandler(currentEvent.ident);
        break;
      case EVFILT_WRITE:
        server->WriteHandler();
        break;
      case EVFILT_TIMER:
        server->TimerHandler();
        break;
      case EVFILT_SIGNAL:
        server->SignalHandler();
        break;
      default:
        break;
	}
}


Master::~Master(void) {}
