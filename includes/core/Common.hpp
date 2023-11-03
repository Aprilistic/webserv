#ifndef COMMON_HPP
#define COMMON_HPP

#include "ConfigMap.hpp"
#include "Core.hpp"
#include "Node.hpp"

class Common {
public:
  static int mKqueue;       // Description for mKqueue
  static bool mRunning;     // Description for mRunning
  static Node *mConfigTree; // Root node for some configuration tree (assuming from the name)
  static ConfigMap *mConfigMap; // A complex map with described structure
};

#endif
