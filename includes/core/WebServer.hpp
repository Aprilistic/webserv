#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Config.hpp"
#include "Core.hpp"
#include "Server.hpp"
#include "Connection.hpp"
#include "ProgramControl.hpp"

class WebServer {
public:
	WebServer(const std::string &path);
	~WebServer(void);
	void Run(void);
	bool IsGood(void) const;

private:
	void eventMonitoring(void);
	
private:
	bool mGood;
	std::map<int, Server *> mServerList;
	std::vector<struct kevent> mEventList;
	ProgramControl mControlInstance;
};

#endif