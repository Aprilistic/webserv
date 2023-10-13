#include "ConfigMap.hpp"

ConfigMap::ConfigMap(Node *configTree) {
  Node *httpNode = configTree->mChildren[0];

  for (size_t i = 0; i < httpNode->mChildren.size(); i++) {
    Node *serverNode = httpNode->mChildren[i];
    int port = 80; // Default port
    if (serverNode->mDirectives.find("listen") != serverNode->mDirectives.end()) {
      port = std::atoi(serverNode->mDirectives["listen"][0].c_str());
    }

    // Port doesn't exist, insert a new PortConfig
    if (mPortConfigs.find(port) == mPortConfigs.end()) {
      mPortConfigs[port] = PortConfig();
    }

    mPortConfigs[port].AddServerConfig(serverNode);
  }
}

Node *ConfigMap::GetConfigNode(int port, const std::string &hostname,
                               const std::string &uri) {
  if (mPortConfigs.find(port) == mPortConfigs.end()) {
    return NULL;
  }

  return mPortConfigs[port].GetConfigNode(hostname, uri);
}