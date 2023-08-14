#include "NgxProcess.hpp"

pid_t NgxProcess::NgxFork(int (*fn)(void *), void *childStack, int flags,
                          void *data) {
  return (clone(fn, childStack, flags, data));
}