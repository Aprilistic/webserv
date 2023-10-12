#ifndef COMMON_HPP
#define COMMON_HPP

#include "Core.hpp"
#include "Node.hpp"

class Common {
public:
    // Nested map definitions for clarity.
    class UriMap : public std::map<std::string, std::string> {};
    class HostnameMap : public std::map<std::string, UriMap> {};

    static int mKqueue;                         // Description for mKqueue
    static bool mRunning;                       // Description for mRunning
    static Node *mConfigTree;                   // Root node for some configuration tree (assuming from the name)
    static HostnameMap complex_map;               // A complex map with described structure

    // ... other members and methods ...
};

#endif
