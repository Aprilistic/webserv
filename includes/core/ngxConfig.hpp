#ifndef NGXCONFIG_HPP
#define NGXCONFIG_HPP

#include "ngxCore.hpp"

class ngxConfig {
  /* methods */
public:
  const int GetWorkerCount() const;
  const int GetWorkerConnectionCount() const;

protected:
private:
  /* variables */
protected:
  /* MainContext */
  int mWorkerCount = 4;

  /* EventsContext */
  int mWorkerConnectionCount = 1024;

  /* HttpContext */

  /* ServerContext */

  /* LocationContext */
};

#endif
