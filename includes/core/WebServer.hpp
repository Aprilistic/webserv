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
	void EventMonitoring(void);
	void EventHandler(struct kevent& currentEvent);
	int GetKqueue(void) const;

private:
	WebServer();
	int mKqueue;
	Node *mConfigTree;
	std::vector<ServerConfig *> mServerConfigList;
	std::vector<struct kevent> mEventList;
};

#endif