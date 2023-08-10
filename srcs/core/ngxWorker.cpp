#include "ngxWorker.hpp"

ngxWorker::ngxWorker(void) : mPID(0), mStatus(0), mServerSocketFd(0){
  openServerSocket();
  createWorkerGroup();
  monitorWorkerGroup();
}

ngxWorker::~ngxWorker(void) {
  closeWorkerGroup();
  removeWorkerGroup();
}

void ngxWorker::openServerSocket(void) {}

void ngxWorker::createWorkerGroup(void) {}

void ngxWorker::monitorWorkerGroup(void) {}

void ngxWorker::closeWorkerGroup(void) {}

void ngxWorker::removeWorkerGroup(void) {}