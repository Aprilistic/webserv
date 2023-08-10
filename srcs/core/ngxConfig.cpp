#include "ngxConfig.hpp"

ngxConfig::ngxConfig()
    : mWorkerCount(4), mWorkerConnectionCount(1024) {}

const int ngxConfig::GetWorkerCount() const { return mWorkerCount; }

const int ngxConfig::GetWorkerConnectionCount() const {
  return mWorkerConnectionCount;
}