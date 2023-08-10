#ifndef NGXWORKER_HPP
#define NGXWORKER_HPP

#include "ngxConfig.hpp"
#include "ngxCore.hpp"

class ngxWorker : public ngxConfig {
public:
  ngxWorker(void);
  ~ngxWorker(void);

private:
  void openServerSocket(void);  /* socket 생성, bind */
  void createWorkerGroup(void); /* worker 생성 */

  void monitorWorkerGroup(void); /* worker 모니터링 */

  void closeServerSocket(void); /* socket close */
  void removeWorkerGroup(void); /* worker remove */

public:
private:
  int mPID;
  int mStatus;
  int mServerSocketFd;
  std::vector<int> mWorkers;
};

#endif