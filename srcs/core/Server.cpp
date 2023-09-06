#include "Server.hpp"

Server::Server(Node* mConfigTree, std::map<int, std::vector<Server*> > mServersMap)
{
	CreateServer(mConfigTree, mServersMap);
	//RunServer(mServers);
}

void Server::CreateServer(Node* mConfigTree, std::map<int, std::vector<Server*> > mServersMap)
{
	std::cout << "Hi" << std::endl;
}

Server::~Server()
{
}