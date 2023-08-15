#include "Master.hpp"

Master::Master(const std::string &path) {
  Config::SetConfig(path);
  startServer();
}

Master::~Master(void) { stopServer(); }

void Master::startServer(void) {}

void Master::stopServer(void) {}
