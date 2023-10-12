#include "ConfigMap.hpp"

ConfigMap::ConfigMap(Node *configTree) {
  Node *httpNode = configTree->mChildren[0];

  for (int i = 0; i < httpNode->mChildren.size(); i++) {
    Node *hostNode = httpNode->mChildren[i];
    setPort(hostNode);
  }
}

void ConfigMap::setPort(const Node *hostNode) {
  std::vector<std::string> portValues = hostNode->mDirectives.at("listen");
  int port = std::atoi(portValues[0].c_str());
  mPortMap.insert(std::make_pair(port, hostNode));
}

void ConfigMap::setHostname(const Node *hostNode, int port) {
  // Assuming hostname directive contains the hostname value
  if (hostNode->mDirectives.count("server_name")) {
    std::string hostname = hostNode->mDirectives.at("server_name")[0];

    // Iterate over URI nodes for this hostname
    for (size_t i = 0; i < hostNode->mChildren.size(); ++i) {
      Node *uriNode = hostNode->mChildren[i];
      setUri(uriNode, port, hostname);
    }
  }
}

void ConfigMap::setUri(const Node *uriNode, int port,
                       const std::string &hostname) {
  std::string uri = uriNode->mDirectives["location"][0];
  mPortMap[port][hostname][uri] = uriNode;
}

ConfigMap::~ConfigMap() { mPortMap.clear(); }
