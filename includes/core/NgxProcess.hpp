#ifndef NGXPORCESS_HPP
#define NGXPORCESS_HPP

#include <csignal>

#include <fcntl.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

#define CLONE_FORK_FLAGS (SIGCHLD)

class NgxProcess {
public:
  static pid_t NgxFork(int (*fn)(void *), void *childStack, int flags, void *data);

private:
  NgxProcess(void);
  ~NgxProcess(void);

public:
private:
};

#endif