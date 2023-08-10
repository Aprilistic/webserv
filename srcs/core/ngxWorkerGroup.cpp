#include "ngxWorkerGroup.hpp"

ngxWorkerGroup::ngxWorkerGroup(void)
    : mPID(0)
    , mStatus(NGX_WORKERGROUP_INIT)
    , mServerSocketFd(0) {
  openServerSocket();
  createWorkerGroup();
  monitorWorkerGroup();
}

ngxWorkerGroup::~ngxWorkerGroup(void) {
  mStatus = NGX_WORKERGROUP_STOP;
  closeServerSocket();
  removeWorkerGroup();
}

const int ngxWorkerGroup::getStatus(void) const { return mStatus; }

void ngxWorkerGroup::openServerSocket(void) {}

void ngxWorkerGroup::createWorkerGroup(void) {
  for (int i = 0; i < mWorkerCount; i++) {
    spawnWorker(0);
  }
  mStatus = NGX_WORKERGROUP_RUN;
}

void ngxWorkerGroup::monitorWorkerGroup(void) {
  while (mStatus == NGX_WORKERGROUP_RUN) {
    if (mStatus == NGX_WORKERGROUP_STOP) {
      assert("interrupt by signal, returned to destructed class");
    }
    int brokenPID = waitpid(-1, &mStatus, WNOHANG);
    if (mStatus == NGX_WORKERGROUP_RUN) {
      spawnWorker(brokenPID);
    }
  }
}

void ngxWorkerGroup::closeServerSocket(void) {
  /* kqueue unregister */
  close(mServerSocketFd);
}

void ngxWorkerGroup::removeWorkerGroup(void) {
  for (int i = 0; i < mWorkerCount; i++) {
    kill(mWorkers[i], SIGQUIT);
  }

  // int retrievedWorkers = 0;
  // while (retrievedWorkers < mWorkerCount) {
  //   int exitedPID = waitpid(-1, &mStatus, WNOHANG);
  //   if (exitedPID > 0) {
  //     retrievedWorkers++;
  //   }
  // }

  // while (mWorkers.size() > 0){
  //   int exitedPID = waitpid(-1, &mStatus, WNOHANG);
  //   if (exitedPID < 0 || std::find(mWorkers.begin(), mWorkers.end(),
  //   exitedPID) == mWorkers.end()) {
  //     assert("worker not found");
  //   }
  //   mWorkers.erase(std::find(mWorkers.begin(), mWorkers.end(), exitedPID));
  // }
}

void ngxWorkerGroup::spawnWorker(int TargetPID) {
  mPID = fork();

  if (mPID < 0) {
    assert("fork() failed");
  } else if (mPID == 0) {
    // child
    doWorkerThings();
  } else {
    // parent
    if (TargetPID == 0) { /* new worker */
      mWorkers.push_back(mPID);
    } else { /* replacement worker */
      std::vector<int>::iterator it =
          std::find(mWorkers.begin(), mWorkers.end(), TargetPID);
      if (it != mWorkers.end()) {
        *it = mPID;
      } else {
        assert("worker not found or in previous group.");
      }
    }
  }
}
