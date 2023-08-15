#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"

#define CONF_BUFFER (4096)

class Config {
  /* methods */
public:
  Config(const std::string &path);
  int SetConfig(const std::string &path);

protected:
private:
  void openConfFile(const std::string &path);
  void parseConfFile(void);

  /* variables */
protected:
private:
  std::stringstream mConfBuffer;
};

#endif