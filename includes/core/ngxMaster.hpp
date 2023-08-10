#ifndef NGXMASTER_HPP
#define NGXMASTER_HPP

#include "ngxConfig.hpp"
#include "ngxCore.hpp"
#include "ngxWorkerGroup.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class ngxMaster {
	public:
	ngxMaster(void);
	~ngxMaster(void);

	private:
	void startNginx(void);
	void reloadNginx(void);
	void stopNginx(void);

	void handleSIGHUP(void);
	void handleSIGQUIT(void);

	public:
	private:
	ngxWorkerGroup *mWorkerGroup;

};

#endif