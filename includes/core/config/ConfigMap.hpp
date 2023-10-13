#ifndef ConfigMap_HPP
#define ConfigMap_HPP

#include "Core.hpp"
#include "Node.hpp"

typedef std::map<std::string, Node *> UriMap;
typedef std::multimap<std::string, UriMap> HostnameMap; // Allows duplicate hostnames

class PortConfig {
public:
  PortConfig();

  void AddServerConfig(Node *serverNode);
  void SetDefaultServer(void); // To set the default server for this port
  Node *GetConfigNode(const std::string &hostname, const std::string &uri);

private:
  UriMap makeUriMap(Node *serverNode);
  void addLocationNode(UriMap *uriConfigs, Node *locationNode);

private:
  HostnameMap mHostnameConfigs;
  HostnameMap::iterator mDefaultServer; // iterator pointing to default server
};

class ConfigMap {
public:
  ConfigMap(Node *configTree);

  Node *GetConfigNode(int port, const std::string &hostname,
                      const std::string &uri);

private:
  std::map<int, PortConfig> mPortConfigs;
};

#endif