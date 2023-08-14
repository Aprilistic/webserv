#include "NgxMaster.hpp"

NgxMaster::NgxMaster(void) { startNginx(); }

NgxMaster::~NgxMaster(void) { stopNginx(); }

void NgxMaster::startNginx(void) { mWorkerGroup = new NgxWorkerGroup(); }

void NgxMaster::reloadNginx(void) {
  NgxWorkerGroup *prevWorkerGroup = mWorkerGroup;
  mWorkerGroup = new NgxWorkerGroup();

  while (mWorkerGroup->GetStatus() == NGX_WORKERGROUP_INIT) {
    usleep(100);
  }
  if (mWorkerGroup->GetStatus() == NGX_WORKERGROUP_RUN) {
    delete prevWorkerGroup;
  } else {
    assert("failed to reload nginx");
  }
}

void NgxMaster::stopNginx(void) { delete mWorkerGroup; }
