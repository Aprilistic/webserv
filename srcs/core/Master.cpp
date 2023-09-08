
#include "Master.hpp"

Master::Master(const std::string &path) {
  //Config config(path, mServersMap)
  startMaster();
}

Master::~Master(void) { stopMaster(); }

void Master::startMaster(void) {

  // Listening socket create
  struct sockaddr_in serv_addr;
  int listenfd;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    std::cout << "socket() error" << std::endl;
    exit(1);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(0);
  serv_addr.sin_port = htons(80);

  // Listening socket bind
  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cout << "bind() error" << std::endl;
    exit(1);
  }

  // Listening socket listen
  if (listen(listenfd, 5) < 0) {
    std::cout << "listen() error" << std::endl;
    exit(1);
  }

  // Listening socket register
  int kq;
  struct kevent evSet;
  struct kevent evList[32];

  kq = kqueue();
  if (kq == -1) {
    std::cout << "kqueue() error" << std::endl;
    exit(1);
  }

  EV_SET(&evSet, listenfd, EVFILT_READ, EV_ADD, 0, 5, NULL);
  if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1) {
    std::cout << "kevent() error" << std::endl;
    exit(1);
  }

  

  // EventMonitor eventMonitor;
  // eventMonitor.Run();
}

void Master::stopMaster(void) {}
