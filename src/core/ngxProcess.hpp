#ifndef NGXPROCESS_HPP
#define NGXPROCESS_HPP

#include "ngxConfig.hpp"
#include "ngxCore.hpp"

class ngxProcess {
  /* methods */
public:
  ngxProcess();
  ~ngxProcess();

  /* initialize */

  /* fork */
  /* -- master -- */
  void SetWorkerGroup(bool updateSignal); /* worker recreate option */
  /* -- worker -- */
  void DoWorkerThings(); /* signal handling - exit request from master, error
                            notification */

  /* waitpid */
  void WaitWorkers(std::vector<int> &prevWorkers); /* retrieve workers */

private:
  void ReloadConfig(); /* signal handling, post&prev replacement */
  void SpawnWorker(int targetPID);
  /* variables */
public:
private:
  int mPID;
  int mStatus;
  int mChannel[2]; /* socket master-worker */
  std::vector<int> mWorkers;
  ngxConfig *mConfig;
};

#endif