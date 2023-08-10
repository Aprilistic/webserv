#include "ngxProcess.hpp"

void ngxProcess::SetWorkerGroup(bool bUpdateSignal) {
  std::vector<int> prevWorkers;
  if (bUpdateSignal){
    prevWorkers = mWorkers;
    mWorkers.clear();
    delete(mConfig);
  }

  mConfig = new ngxConfig();
  int workerCount = mConfig->GetWorkerCount();
  for (int index = 0; index < workerCount; index++) {
    spawnWorker(0);
  }

  if (bUpdateSignal){
    kqueue_massage(); /* */
    waitWorkers(prevWorkers);
  }
}

void ngxProcess::waitWorkers(std::vector<int> &workers)
{
}

void ngxProcess::handleSIGHUP() {
  SetWorkerGroup(true);
}

void ngxProcess::handleSIGQUIT() {
  waitWorkers(mWorkers);
}