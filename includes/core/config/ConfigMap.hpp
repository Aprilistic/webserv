#ifndef ConfigMap_HPP
#define ConfigMap_HPP

#include "Core.hpp"
#include "Node.hpp"

class ConfigMap {
public:
  typedef std::map<std::string, Node *> UriMap;
  typedef std::multimap<std::string, UriMap> HostnameMap;
  typedef std::map<int, HostnameMap> PortMap;

  ConfigMap(Node *configTree);
  ~ConfigMap();

  Node *getConfigNode(int port, const std::string &hostname,
                      const std::string &uri);

private:
  void setPort(const Node *hostNode);
  HostnameMap setHostname(const Node *hostNode, int port);
  UriMap setUri(const Node *uriNode, int port, const std::string &hostname);

private:
  PortMap mPortMap;
};

#endif