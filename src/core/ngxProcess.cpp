#include "ngxProcess.hpp"

ngxProcess::ngxProcess() : mPID(0), mStatus(0) {}

ngxProcess::~ngxProcess() { delete mConfig; }

void ngxProcess::SpawnWorker(int targetPID) {
  mPID = fork();

  if (mPID < 0) {
    assert("fork() failed");
  } else if (mPID == 0) {
    // child
    DoWorkerThings();
  } else {
    // parent
    if (targetPID == 0) { /* new worker */
      mWorkers.push_back(mPID);
    } else { /* replacement worker */
      std::vector<int>::iterator it =
          find(mWorkers.begin(), mWorkers.end(), targetPID);
      if (it != mWorkers.end()) {
        *it = mPID;
      } else {
        assert("worker not found or in previous group.");
      }
    }
  }
}
