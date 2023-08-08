#include "ngxProcess.hpp"

void ngxProcess::SetWorkerGroup(bool updateSignal) {
  std::vector<int> prevWorkers;
  if (updateSignal){
    prevWorkers = mWorkers;
    mWorkers.clear();
    delete(mConfig);
  }

  mConfig = new ngxConfig();
  int workerCount = mConfig->GetWorkerCount();
  for (int index = 0; index < workerCount; index++) {
    SpawnWorker(0);
  }

  if (updateSignal){
    kqueue_massage(); /* */
    WaitWorkers(prevWorkers);
  }
}