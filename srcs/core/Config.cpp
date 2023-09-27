#include "Config.hpp"

Node *Config::makeConfigTree(const std::string &path) {
  Node *configTree;

  try {
    std::vector<std::string> configTokens = Tokenizer::makeTokens(path);
    std::vector<std::string>::iterator tokenLocation = configTokens.begin();
    configTree = new Node(configTokens, tokenLocation, NULL, 1);

    checkSum(configTree);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;

    // If there is an error, delete the tree and return NULL
    return (NULL);
  }

  return (configTree);
}

void Config::checkSum(Node *configTree) { configTree->CheckSum(0); }

std::map<int, ServerConfig *> Config::makeServerConfigList(WebServer *webServer,
                                                         Node *configTree) {
  std::map<int, ServerConfig *> serverConfigList;
  try {
    if (!configTree->mChildren.empty()) {
      Node *httpNode = configTree->mChildren[0];

      for (std::vector<Node *>::iterator serverNode =
               httpNode->mChildren.begin();
           serverNode != httpNode->mChildren.end(); ++serverNode) {
		ServerConfig* newConfig = new ServerConfig(webServer, *serverNode);
        serverConfigList[newConfig->mSocket] = newConfig;
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
	for (std::map<int, ServerConfig*>::iterator it = serverConfigList.begin();
			it != serverConfigList.end(); ++it) {
		delete it->second;
	}
    serverConfigList.clear();
  }

  return (serverConfigList);
}