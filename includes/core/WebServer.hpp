#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Config.hpp"
class Server; 

class WebServer
{
	public:
		WebServer(const std::string& path);
		void StartServer(const std::string& path);
		void StopServer(void);
		void DeleteTree(void);
		//void DeleteServers(void);
		~WebServer(void);
	private:
		Node* mConfigTree;
		std::map<int, std::vector<Server*> > mServersMap;
		// std::vector<int, std::vector<class ioConnection*> > mConnections;
};

#endif
