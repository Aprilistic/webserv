#ifndef ConfigMap_HPP
#define ConfigMap_HPP

#include "Core.hpp"
#include "Node.hpp"

class ConfigMap {
public:
  typedef std::map<std::string, Node *> UriMap;
  typedef std::multimap<std::string, UriMap>
      HostnameMap; // Allows duplicate hostnames

  ConfigMap(Node *configTree);
  Node *GetConfigNode(int port, const std::string &hostname,
                      const std::string &uri, const std::string &method);
  const std::vector<int> GetPorts() const;

private:
  class PortMap {
  public:
    PortMap();

    void AddServerConfig(Node *serverNode);
    Node *GetConfigNode(const std::string &hostname, const std::string &uri,
                        const std::string &method);

  private:
    bool checkCGIMethos(const std::string &method, Node *locationNode);
    Node *miniPCRE(UriMap *uriConfigs, const std::string uri,
                   const std::string &method);
    Node *longestMatchedNode(UriMap *uriConfigs, const std::string uri);
    Node *searchInServerConfig(UriMap *uriConfigs, const std::string &uri,
                               const std::string &method);
    UriMap makeUriMap(Node *serverNode);
    void addLocationNode(UriMap *uriConfigs, Node *locationNode);

  private:
    HostnameMap mHostnameConfigs;
    UriMap *mDefaultServer; // pointer to default server config
    bool mbDefaultServerSet;
  };

  std::map<int, PortMap> mPortConfigs;
};

#endif
