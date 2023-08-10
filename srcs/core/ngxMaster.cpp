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

	if (mWorkerGroup->getStatus() == NGX_WORKERGROUP_RUN) {
		delete mWorkerGroup;
		mWorkerGroup = prevWorkerGroup;
		return;
	}
}

void ngxMaster::stopNginx(void) {
	delete mWorkerGroup;
}
