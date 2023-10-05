#define SERVER_LEVEL (1 << 2)
#define LOCATION_LEVEL (1 << 3)

#include "Server.hpp"
#include "WebServer.hpp"

Server::Server(Node *ServerNode)
    : mServerNode(ServerNode) {

  makeLocationHashMap(ServerNode);

  // printHashMap();

  mSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (mSocket < 0) {
    //FD limit exceeded
    throw std::runtime_error("Error: socket() creation failed: " + std::string(strerror(errno)));
  }

  fcntl(mSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

  memset(&mAddr, 0, sizeof(mAddr));
  mAddr.sin_family = AF_INET;
  mAddr.sin_port = htons(mPort);
  mAddr.sin_addr.s_addr = htonl(0);

  if (bind(mSocket, (struct sockaddr *)&mAddr, sizeof(mAddr)) < 0) {
    throw std::runtime_error("Error: Failed to bind the socket: " + std::string(strerror(errno)));
  }

  if (listen(mSocket, SOMAXCONN /* backlog size*/) < 0) {
    throw std::runtime_error("Error: Failed to listen on the socket: " + std::string(strerror(errno)));
  }

  struct kevent events[2];

  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);

//   struct timespec *timeout = NULL; // wait indefinitely
//   int n = kevent(mWebServer->GetKqueue(), NULL, 0, events, 1, timeout);
}

Server::~Server() {
  // Remove the socket from the mKqueue
  struct kevent events[2];
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(Common::mKqueue, events, 2, NULL, 0, NULL);

  // Close the socket
  for (std::map<int, Connection *>::iterator it = mConnection.begin();
       it != mConnection.end(); ++it) {
    delete it->second;
  }

  close(mSocket);
}

void Server::makeLocationHashMap(Node *curNode) {
  if (curNode->mLevel & SERVER_LEVEL) {
    assert(curNode->mChildren.size());
  } else if (curNode->mLevel & LOCATION_LEVEL) {
    std::pair<std::string, Node *> LocDict =
        std::make_pair(curNode->mDirectives["location"][0], curNode);
    mLocationHashMap.insert(LocDict);
  }

  for (std::vector<Node *>::iterator it = curNode->mChildren.begin();
       it != curNode->mChildren.end(); ++it) {
    makeLocationHashMap(*it);
  }
}

void Server::HandleReadEvent()
{
	int socket = accept(mSocket, NULL, NULL);

	if (socket == -1)
	{
		//error
	}
	mConnection[socket] = new Connection(socket);
}


void Server::HandleWriteEvent()
{
}

void Server::HandleTimerEvent()
{
}

void Server::HandleSignalEvent()
{
}

// void Server::printHashMap() {
//   std::cout << "Location HashMap" << std::endl;
//   std::map<std::string, Node *>::iterator it;
//   for (it = mLocationHashMap.begin(); it != mLocationHashMap.end(); ++it) {
//     std::cout << "Key: " << it->first << ", Node Address: " << it->second
//               << std::endl;
//   }
// }
