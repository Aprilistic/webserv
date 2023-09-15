#include "Master.hpp"

Master::Master(const std::string &path) {
  Config config(path);
  EventMonitor eventMonitor(config.GetConfigTree());
}

Master::~Master(void) {}

