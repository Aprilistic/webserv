#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"

#define CONF_BUFFER (4096)

class Config {
  /* methods */
public:
  Config(const std::string &path);

protected:
private:
  bool openConfFile(const std::string &path);
  void parseConfFile(void);

  /* variables */
protected:
private:
  std::stringstream mConfBuffer;
};

#endif