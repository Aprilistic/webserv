#include "WebServer.hpp"
#include "Server.hpp"

WebServer::WebServer(const std::string& path)
{
	StartServer(path);
}

void WebServer::StartServer(const std::string& path)
{
	Config config(path, mConfigTree);
	Server server(mConfigTree, mServersMap);
	// IoConnection connection(mServers, mConnections);
}

void WebServer::StopServer(void)
{
}

void WebServer::DeleteTree(void)
{
	delete mConfigTree;
}

WebServer::~WebServer(void)
{
	DeleteTree();
}
