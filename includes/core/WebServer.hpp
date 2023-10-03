#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Config.hpp"
#include "Core.hpp"
#include "Server.hpp"
#include "Connection.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class WebServer {
public:
	WebServer(const std::string &path);
	~WebServer(void);
	void Run(void);
	bool IsGood(void) const;

private:
	void eventMonitoring(void);
	void eventHandler(struct kevent& currentEvent);
	
private:
	bool mGood;
	std::map<int, Server *> mServerList;
	std::vector<struct kevent> mEventList;
};

#endif