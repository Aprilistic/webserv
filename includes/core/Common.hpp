#ifndef COMMON_HPP
#define COMMON_HPP

#include "Core.hpp"
#include "Node.hpp"

class Common {
public:
  static int mKqueue;
  static bool mRunning;
  static Node *mConfigTree;
};

#endif