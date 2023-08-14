#ifndef NGXCONFIG_HPP
#define NGXCONFIG_HPP

#include "NgxCore.hpp"

class NgxConfig {
  /* methods */
public:
  NgxConfig();

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
