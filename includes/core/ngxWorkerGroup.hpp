#ifndef NGXWORKERGROUP_HPP
#define NGXWORKERGROUP_HPP

#include "ngxConfig.hpp"
#include "ngxCore.hpp"

#define NGX_WORKERGROUP_INIT (0)
#define NGX_WORKERGROUP_RUN (1)
#define NGX_WORKERGROUP_STOP (2)

class ngxWorkerGroup : public ngxConfig {
public:
  ngxWorkerGroup(void);
  ~ngxWorkerGroup(void);

  const int getStatus(void) const;

private:
  void openServerSocket(void);  /* socket 생성, bind */
  void createWorkerGroup(void); /* worker 생성 */

  void monitorWorkerGroup(void); /* worker 모니터링 */

  void closeServerSocket(void); /* socket close */
  void removeWorkerGroup(void); /* worker remove */

  void spawnWorker(int TargetPID);
  void doWorkerThings(void);

public:
private:
  int mPID;
  int mStatus;
  int mServerSocketFd;
  std::vector<int> mWorkers;
};

#endif