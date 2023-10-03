#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Core.hpp"
#include "ServerConfig.hpp"
#include "Client.hpp"
#include "Common.hpp"

/* Signal hadling SIGHUP, SIGQUIT or SIGTERM */

class WebServer {
public:
	WebServer(const std::string &path);
	~WebServer(void);
	void Run(void);
	bool IsGood(void) const;

private:
	WebServer();
	void eventMonitoring(void);
	void eventHandler(struct kevent& currentEvent);
	void handleReadEvent(struct kevent& currentEvent);
	void handleWriteEvent(struct kevent& currentEvent);
	// void handleTimerEvent(struct kevent& currentEvent);
	// void handleSignalEvent(struct kevent& currentEvent);

	void onServerRead(int ident);
	void onClientRead(int ident);
	void onServerWrite(int ident);
	void onClientWrite(int ident);
public:

private:
	bool mGood;
	// std::vector<ServerConfig *> mServerConfigList;
	std::map<int, ServerConfig *> mServerConfigList;
	// std::vector<Client *> mClientList;
	std::map<int, Client *> mClientList;
	std::vector<struct kevent> mEventList;
};

#endif