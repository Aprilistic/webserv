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
  void BindSocket(); /* lintening server socket */
  void SetWorkerGroup(bool bUpdateSignal); /* worker recreate option */
  void MonitorWorkers(); /* waitpid - replace worker process */
  /* -- worker -- */
  void 
  void DoWorkerThings(); /* signal handling - exit request from master, error
                            notification */

private:
  /* -- master -- */
  void reloadConfig(); /* signal handling, post&prev replacement */
  void spawnWorker(int targetPID);
  void waitWorkers(std::vector<int> &workers); /* retrieve workers */

  void handleSIGHUP(); /* reload signal handling */
  void handleSIGQUIT(); /* exit signal handling */
  // void handleSIGTERM(); /* terminate signal handling */
  /* -- worker -- */
  
/* variables */
public:
private:
  int mPID;
  int mStatus;
  int mChannel[2]; /* socket master-worker */
  // std::vector<std::vector<int>> mChannel; /* socket master-worker */
  std::vector<int> mWorkers;
  ngxConfig *mConfig;
};

#endif