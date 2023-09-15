#include "EventMonitor.hpp"
#include "Server.hpp"

EventMonitor::EventMonitor(Node *mConfigTree) {
  mKqueue = kqueue();
  mQuit = 0;

  Node *httpNode = mConfigTree->mChildren[0];
  std::cout<<"Node lv: " << httpNode->mLevel<<std::endl;
  for (std::vector<Node *>::iterator serverNode = httpNode->mChildren.begin();
       serverNode != httpNode->mChildren.end(); ++serverNode) {
          std::cout<<"ServerNode lv: " << (*serverNode)->mLevel<<std::endl;
    mServerList.push_back(new Server(this, *serverNode));
  }

}

//signal, timer registeration to kqueue

void EventMonitor::MonitorIO() {
  while (mQuit == 0) {
    int nev = kevent(mKqueue, NULL, 0, mEvList, 32, NULL);

    if (nev < 0 && errno == EINTR) {
      continue;
    }

    for (int i = 0; i < nev; i++) {
      Server *server = (Server *)mEvList[i].udata;

      // if (mEvList[i].flags & EV_ERROR)
      // {
      //   // error
      // }
      // else if (mEvList[i].filter == EVFILT_READ) {
      //   if (mEvList[i].ident == server->mSocket)
      //   {
      //       int clientSocket;
      //       if ((clientSocket = accept(server->mSocket, NULL, NULL)) == -1)
      //       {
      //         // error
      //       }
      //       fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
      //   }
      //   server->ReadHandler();
      // } else if (mEvList[i].filter == EVFILT_WRITE) {
      //   server->WriteHandler();
      // }
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
