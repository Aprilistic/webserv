#include "Config.hpp"

Config::Config() : mWorkerCount(4), mWorkerConnectionCount(1024) {}

const int Config::GetWorkerCount() const { return mWorkerCount; }

const int Config::GetWorkerConnectionCount() const {
  return mWorkerConnectionCount;
}