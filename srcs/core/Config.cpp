#include "Config.hpp"

Config::Config(const std::string &path) {
  if (openConfFile(path) == false)
    throw std::runtime_error("Could not open confFile");
}

bool Config::openConfFile(const std::string &path) {
  std::ifstream confFile(path);
  if (confFile.is_open() == false) {
    std::cerr << "Error: Could not open confFile " << path << std::endl;
    return false;
  }

  mConfBuffer << confFile.rdbuf();
  confFile.close();
  if (mConfBuffer.good() == false) {
    std::cerr << "Error: confFile stream " << path << std::endl;
    return false;
  }
  return true;
}