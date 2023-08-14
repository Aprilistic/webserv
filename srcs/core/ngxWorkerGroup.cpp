#include "NgxWorkerGroup.hpp"

#define QUIT_WHEN_NOCHILD (0)
#define RECOVER_IF_FAIL (1)

NgxWorkerGroup::NgxWorkerGroup(void)
    : mPID(0), mProcessType(NGX_PROCESS_MASTER),
      mWorkerGroupStatus(NGX_WORKERGROUP_INIT), mServerSocketFd(0) {
  openServerSocket();
  createWorkerGroup();
}

NgxWorkerGroup::~NgxWorkerGroup(void) {
  mWorkerGroupStatus = NGX_WORKERGROUP_STOP;
  closeServerSocket();
  removeWorkerGroup();
}

const int NgxWorkerGroup::GetStatus(void) const { return mWorkerGroupStatus; }

void NgxWorkerGroup::openServerSocket(void) {}
  
void NgxWorkerGroup::createWorkerGroup(void) {
  for (int i = 0; i < mWorkerCount; i++) {
    spawnWorker(0);
  }
  mWorkerGroupStatus = NGX_WORKERGROUP_RUN;
  waitWorker(RECOVER_IF_FAIL);
}

void NgxWorkerGroup::closeServerSocket(void) {
  /* kqueue unregister */
  close(mServerSocketFd);
}

void NgxWorkerGroup::removeWorkerGroup(void) {
  mWorkerGroupStatus = NGX_WORKERGROUP_STOP;
  for (int i = 0; i < mWorkerCount; i++) {
    kill(mWorkers[i], SIGQUIT); /* ⭐️⭐️⭐️⭐️⭐️ */
  }
  waitWorker(QUIT_WHEN_NOCHILD);
}

void NgxWorkerGroup::spawnWorker(int targetPID) {
  mPID = fork();

  if (mPID < 0) {
    assert("fork() failed");
  } else if (mPID == 0) {
    // child
    doWorkerThings();
  } else {
    // parent
    if (targetPID == 0) { /* new worker */
      mWorkers.push_back(mPID);
    } else { /* replacement worker */
      std::vector<int>::iterator it =
          std::find(mWorkers.begin(), mWorkers.end(), targetPID);
      if (it != mWorkers.end()) {
        *it = mPID;
      } else {
        assert("worker not found or in previous group.");
      }
    }
  }
}

void NgxWorkerGroup::waitWorker(int mode) {
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
      } else if (mode == QUIT_WHEN_NOCHILD && errno == ECHILD) {
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
      if (mode == RECOVER_IF_FAIL &&
          mWorkerGroupStatus == NGX_WORKERGROUP_RUN) {
        spawnWorker(pid);
      }
    }
  }
}