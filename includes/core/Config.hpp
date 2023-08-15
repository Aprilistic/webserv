#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Core.hpp"

class Config {
  /* methods */
public:
  Config();

  const int GetWorkerCount() const;
  const int GetWorkerConnectionCount() const;

protected:
private:
  /* variables */
protected:
  /* MainContext */
  int mWorkerCount;

  /* EventsContext */
  int mWorkerConnectionCount;

  /* HttpContext */

  /* ServerContext */
  /* LocationContext */
};

#endif
