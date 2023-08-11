#include "ngxWorkerGroup.hpp"

#define QUIT_WHEN_NOCHILD (0)
#define RECOVER_IF_FAIL (1)

ngxWorkerGroup::ngxWorkerGroup(void)
    : mPID(0), mStatus(NGX_WORKERGROUP_INIT), mServerSocketFd(0) {
  openServerSocket();
  createWorkerGroup();
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
  waitWorker(RECOVER_IF_FAIL);
}

void ngxWorkerGroup::closeServerSocket(void) {
  /* kqueue unregister */
  close(mServerSocketFd);
}

void ngxWorkerGroup::removeWorkerGroup(void) {
  for (int i = 0; i < mWorkerCount; i++) {
    kill(mWorkers[i], SIGQUIT); /* ⭐️⭐️⭐️⭐️⭐️ */
  }
  waitWorker(QUIT_WHEN_NOCHILD);
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

void ngxWorkerGroup::waitWorker(int Mode) {
  int pid;
  int status;

  while (true) {
    pid = waitpid(-1, &status, WNOHANG);

    if (pid == 0) {
      continue;
    }

    if (pid < 0) {
      if (errno == EINTR) {
        continue;
      } else if (Mode == QUIT_WHEN_NOCHILD && errno == ECHILD) {
        break;
      } else {
        assert("waitpid() failed");
      }
    }

    if (pid > 0) {
      if (WTERMSIG(status)) {
        assert("worker terminated by signal"); /* ⭐️⭐️⭐️⭐️⭐️ */
      }
      if (WEXITSTATUS(status)) {
        assert("worker exited with non-zero status"); /* ⭐️⭐️⭐️⭐️⭐️ */
      }
      if (Mode == RECOVER_IF_FAIL && mStatus == NGX_WORKERGROUP_RUN) {
        spawnWorker(pid);
      }
    }
  }
}