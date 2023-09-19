#include "EventMonitor.hpp"
#include "Server.hpp"

EventMonitor::EventMonitor(Node *mConfigTree) {
  mKqueue = kqueue();
  mQuit = 0;

  Node *httpNode = mConfigTree->mChildren[0];
  for (std::vector<Node *>::iterator serverNode = httpNode->mChildren.begin();
       serverNode != httpNode->mChildren.end(); ++serverNode) {
    mServerList.push_back(new Server(this, *serverNode));
  }
}

// signal, timer registeration to kqueue

void EventMonitor::MonitorIO() {
  while (mQuit == 0) {
    int nev = kevent(mKqueue, NULL, 0, mEvList, 32, NULL);

    if (nev < 0 && errno == EINTR) {
      continue;
    }

    for (int i = 0; i < nev; i++) {
      Server *server = (Server *)mEvList[i].udata;

      if (mEvList[i].flags & EV_ERROR) {
        //error client으ㅣ 경경우  disconnect 필요
      }

      switch (mEvList[i].filter) {
      case EVFILT_READ:
        if (mEvList[i].ident == server->mSocket) {
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
        server->ReadHandler(mEvList[i].ident);
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
  }
}

EventMonitor::~EventMonitor() {
  for (std::vector<Server *>::iterator it = mServerList.begin();
       it != mServerList.end(); it++) {
    delete *it;
  }
  close(mKqueue);
}

struct kevent EventMonitor::GetEvList() { return *mEvList; }