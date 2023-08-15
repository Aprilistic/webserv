#include "Config.hpp"

std::map<std::string, > Config::SetConfig(const std::string &path) {
  openConfFile(path);
  return (parseConfFile());
}

void Config::openConfFile(const std::string &path) {
  std::ifstream confFile(path);
  if (confFile.is_open() == false) {
    std::cerr << "Error: Could not open confFile " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  mConfBuffer << confFile.rdbuf();
  confFile.close();
  if (mConfBuffer.good() == false) {
    std::cerr << "Error: confFile stream " << path << std::endl;
    exit(EXIT_FAILURE);
  }
}

std::map<std::string, ServerInfo>  Config::parseConfFile() {}