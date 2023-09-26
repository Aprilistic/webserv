#include "Config.hpp"

Node* Config::makeConfigTree(const std::string &path)
{
	std::vector<std::string> Tokens = Token::makeTokens(path);
	Node *ConfigTree;

	std::vector<std::string>::iterator token = Tokens.begin();
	ConfigTree = new Node(Tokens, token, NULL, 1);

	checkSum(ConfigTree);
	
	return (ConfigTree);
}

void Config::checkSum(Node *ConfigTree)
{
  ConfigTree->checkSum(0);
}


std::vector<ServerConfig *> Config::makeServerConfigList(WebServer *webServer, Node *configTree)
{
	std::vector<ServerConfig *> serverConfigList;

	Node* httpNode = configTree->mChildren[0];

	for (std::vector<Node *>::iterator serverNode = httpNode->mChildren.begin();
		serverNode != httpNode->mChildren.end(); ++serverNode)
		{
			serverConfigList.push_back(new ServerConfig(webServer, *serverNode));
		}

	return (serverConfigList);
}
