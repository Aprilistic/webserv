#include "ConfigMap.hpp"

PortConfig::PortConfig() : mDefaultServer(mHostnameConfigs.end()) {}

void PortConfig::AddServerConfig(Node *serverNode) {
  std::string hostname = "localhost"; // Default hostname
  if (serverNode->mDirectives.find("server_name") !=
      serverNode->mDirectives.end()) {
    hostname = serverNode->mDirectives["server_name"][0];
  }

  mHostnameConfigs.insert(
      std::pair<std::string, UriMap>(hostname, makeUriMap(serverNode)));
}

UriMap PortConfig::makeUriMap(Node *serverNode) {
  UriMap uriConfigs;
  addLocationNode(&uriConfigs, serverNode);
  return uriConfigs;
}

void PortConfig::addLocationNode(UriMap *uriConfigs, Node *serverNode) {
  for (size_t i = 0; i < serverNode->mChildren.size(); i++) {
    Node *locationNode = serverNode->mChildren[i];
    std::string uri = locationNode->mDirectives["location"][0];
    uriConfigs->insert(std::pair<std::string, Node *>(uri, locationNode));
    PortConfig::addLocationNode(uriConfigs, locationNode);
  }
}

ConfigMap::ConfigMap(Node *configTree) {
  Node *httpNode = configTree->mChildren[0];

  for (size_t i = 0; i < httpNode->mChildren.size(); i++) {
    Node *serverNode = httpNode->mChildren[i];
    int port = 80; // Default port
    if (serverNode->mDirectives.find("listen") !=
        serverNode->mDirectives.end()) {
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