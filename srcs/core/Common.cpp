#include "Common.hpp"

int Common::mKqueue = 0;
bool Common::mRunning = false;
Node *Common::mConfigTree = NULL;
ConfigMap *Common::mConfigMap = NULL;