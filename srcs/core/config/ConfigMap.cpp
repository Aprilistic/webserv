#include "ConfigMap.hpp"

ConfigMap::PortMap::PortMap()
    : mDefaultServer(NULL), mbDefaultServerSet(false) {}

void ConfigMap::PortMap::AddServerConfig(Node *serverNode) {
  std::string hostname = "localhost"; // Default hostname
  if (serverNode->mDirectives.find("server_name") !=
      serverNode->mDirectives.end()) {
    hostname = serverNode->mDirectives["server_name"][0];
  }

  // Insert a new UriMap
  HostnameMap::iterator insertedItem = mHostnameConfigs.insert(
      std::pair<std::string, UriMap>(hostname, makeUriMap(serverNode)));

  // Set default server if it's the first server
  if (mHostnameConfigs.size() == 1) {
    mDefaultServer = &(insertedItem->second);
  }

  if (serverNode->mDirectives.find("listen") == serverNode->mDirectives.end()) {
    return; // Early return if 'listen' directive doesn't exist
  }
  std::vector<std::string> &listenDirectives =
      serverNode->mDirectives["listen"];
  if (std::find(listenDirectives.begin(), listenDirectives.end(),
                "default_server") != listenDirectives.end()) {
    if (mbDefaultServerSet) {
      throw std::runtime_error("Multiple default servers specified");
    }
    mDefaultServer = &(insertedItem->second);
    mbDefaultServerSet = true;
  }
}

Node *ConfigMap::PortMap::GetConfigNode(const std::string &hostname,
                                        const std::string &uri) {
  Node *configNode = NULL;

  // Search URI in hostname config
  HostnameMap::iterator it = mHostnameConfigs.find(hostname);
  if (it != mHostnameConfigs.end()) {
    configNode = searchInServerConfig(&(it->second), uri);
    if (configNode != NULL) {
      return configNode;
    }
  }

  // If not found, search URI in default server config
  if (&(it->second) != mDefaultServer) {
    configNode = searchInServerConfig(mDefaultServer, uri);
    if (configNode != NULL) {
      return configNode;
    }
  }

  return NULL; // node not found 404
}

const std::vector<int> ConfigMap::GetPorts() const {
  std::vector<int> mPorts;
  for (std::map<int, ConfigMap::PortMap>::const_iterator it =
           mPortConfigs.begin();
       it != mPortConfigs.end(); ++it) {
    mPorts.push_back(it->first);
  }
  return mPorts;
}

Node *ConfigMap::PortMap::searchInServerConfig(UriMap *uriConfigs,
                                               const std::string &uri) {
  if (uriConfigs->find(uri) != uriConfigs->end()) { // URI found
    return (*uriConfigs)[uri];
  }
  if (uriConfigs->find("/") != uriConfigs->end()) { // URI not found, search "/"
    return (*uriConfigs)["/"];
  }
  return NULL; // URI not found, 404
}

ConfigMap::UriMap ConfigMap::PortMap::makeUriMap(Node *serverNode) {
  UriMap uriConfigs;
  addLocationNode(&uriConfigs, serverNode);
  return uriConfigs;
}

void ConfigMap::PortMap::addLocationNode(UriMap *uriConfigs, Node *serverNode) {
  for (size_t i = 0; i < serverNode->mChildren.size(); i++) {
    Node *locationNode = serverNode->mChildren[i];
    std::string uri = locationNode->mDirectives["location"][0];
    uriConfigs->insert(std::pair<std::string, Node *>(uri, locationNode));
    PortMap::addLocationNode(uriConfigs, locationNode);
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

    // Port doesn't exist, insert a new PortMap
    if (mPortConfigs.find(port) == mPortConfigs.end()) {
      mPortConfigs[port] = PortMap();
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