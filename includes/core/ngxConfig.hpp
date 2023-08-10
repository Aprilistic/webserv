#ifndef NGXCONFIG_HPP
#define NGXCONFIG_HPP

#include "ngxCore.hpp"

class ngxConfig {
  /* methods */
public:
  ngxConfig();

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
