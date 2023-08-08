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
  void ReloadConfig(); /* signal handling, post&prev replacement */

  /* fork */
  /* -- master -- */
  void SpawnWorkers(); /* worker recreate option */
  /* -- worker -- */
  void DoWorkerThings(); /* signal handling - exit request from master, error notification */

  /* waitpid */
  void WaitWorkers(vector<int> &prevWorkers); /* retrieve workers */

private:

/* variables */
public:
private:
  int mPid;
  int mStatus;
  int mChannel[2]; /* socket master-worker */
  vector<int> mPrevWorkers;
  vector<int> mPostWorkers;
  ngxConfig *mPrevConfig;
  ngxConfig *mPostConfig = NULL;
};

#endif