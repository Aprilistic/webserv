#include "ngxProcess.hpp"

ngxProcess::ngxProcess() : mPid(0), mStatus(0) {
  mPrevConfig = new ngxConfig();
}

ngxProcess::~ngxProcess() { delete mPrevConfig; }