#define SERVERCONFIG_LEVEL (1 << 2)
#define LOCATIONCONFIG_LEVEL (1 << 3)

#include "ServerConfig.hpp"
#include "WebServer.hpp"

ServerConfig::ServerConfig(WebServer *webServer, Node *serverConfigNode)
    : mWebServer(webServer)
    , mServerConfigNode(serverConfigNode) {

  makeLocationConfigHashMap(serverConfigNode);

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

  if (bind(mSocket, (struct sockaddr *)&mAddr, sizeof(mAddr) < 0)) {
    throw std::runtime_error("Error: Failed to bind the socket: " + std::string(strerror(errno)));
  }

  if (listen(mSocket, SOMAXCONN /* backlog size*/) < 0) {
    throw std::runtime_error("Error: Failed to listen on the socket: " + std::string(strerror(errno)));
  }

  struct kevent events[2];

  EV_SET(&events[0], mSocket, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, this);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, this);
  kevent(mWebServer->GetKqueue(), events, 2, NULL, 0, NULL);

//   struct timespec *timeout = NULL; // wait indefinitely
//   int n = kevent(mWebServer->GetKqueue(), NULL, 0, events, 1, timeout);
}

ServerConfig::~ServerConfig() {
  // Remove the socket from the mKqueue
  struct kevent events[2];
  EV_SET(&events[0], mSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&events[1], mSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  kevent(mWebServer->GetKqueue(), events, 2, NULL, 0, NULL);

  // Close the socket
  close(mSocket);
}

void ServerConfig::makeLocationConfigHashMap(Node *curNode) {
  if (curNode->mLevel & SERVERCONFIG_LEVEL) {
    assert(curNode->mChildren.size());
  } else if (curNode->mLevel & LOCATIONCONFIG_LEVEL) {
    std::pair<std::string, Node *> LocDict =
        std::make_pair(curNode->mDirectives["location"][0], curNode);
    mLocationConfigHashMap.insert(LocDict);
  }

  for (std::vector<Node *>::iterator it = curNode->mChildren.begin();
       it != curNode->mChildren.end(); ++it) {
    makeLocationConfigHashMap(*it);
  }
}

// void ServerConfig::printHashMap() {
//   std::cout << "Location HashMap" << std::endl;
//   std::map<std::string, Node *>::iterator it;
//   for (it = mLocationConfigHashMap.begin(); it != mLocationConfigHashMap.end(); ++it) {
//     std::cout << "Key: " << it->first << ", Node Address: " << it->second
//               << std::endl;
//   }
// }