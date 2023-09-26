#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Config.hpp"
#include "Core.hpp"
#include "ServerConfig.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class WebServer {
public:
	WebServer(const std::string &path);
	~WebServer(void);
	void Run(void);
	int GetKqueue(void) const;
	bool IsGood(void) const;

private:
	WebServer();
	void eventMonitoring(void);
	void eventHandler(struct kevent& currentEvent);

private:
	bool mGood;
	int mKqueue;
	Node *mConfigTree;
	std::vector<ServerConfig *> mServerConfigList;
	std::vector<struct kevent> mEventList;
};

#endif