#include "NgxConfig.hpp"

NgxConfig::NgxConfig() : mWorkerCount(4), mWorkerConnectionCount(1024) {}

const int NgxConfig::GetWorkerCount() const { return mWorkerCount; }

const int NgxConfig::GetWorkerConnectionCount() const {
  return mWorkerConnectionCount;
}