#ifndef NGXWORKERGROUP_HPP
#define NGXWORKERGROUP_HPP

#include "NgxConfig.hpp"
#include "NgxCore.hpp"

#define NGX_WORKERGROUP_INIT (0)
#define NGX_WORKERGROUP_RUN (1)
#define NGX_WORKERGROUP_STOP (2)

class NgxWorkerGroup : public NgxConfig {
public:
  NgxWorkerGroup(void);
  ~NgxWorkerGroup(void);

  const int GetStatus(void) const;

private:
  void openServerSocket(void);  /* socket 생성, bind */
  void createWorkerGroup(void); /* worker 생성 */

  void closeServerSocket(void); /* socket close */
  void removeWorkerGroup(void); /* worker remove */

  void spawnWorker(int targetPID);
  void waitWorker(int mode);

  void doWorkerThings(void);

public:
private:
  int mPID;
  int mProcessType;
  int mWorkerGroupStatus;
  int mServerSocketFd;
  std::vector<int> mWorkers;
};

#endif