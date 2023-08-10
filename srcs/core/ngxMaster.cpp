#include "ngxMaster.hpp"

ngxMaster::ngxMaster(void) {
	startNginx();
}

ngxMaster::~ngxMaster(void) {
	stopNginx();
}

void ngxMaster::startNginx(void) {
	mWorkerGroup = new ngxWorkerGroup();
}

void ngxMaster::reloadNginx(void) {
	ngxWorkerGroup *prevWorkerGroup = mWorkerGroup;
	mWorkerGroup = new ngxWorkerGroup();
	// check
	while (mWorkerGroup->getStatus() == NGX_WORKERGROUP_INIT) {
		usleep(100);
	}
	if (mWorkerGroup->getStatus() == NGX_WORKERGROUP_RUN) {
		delete prevWorkerGroup;
	} else {
		assert("failed to reload nginx");
	}
}

void ngxMaster::stopNginx(void) {
	delete mWorkerGroup;
}
