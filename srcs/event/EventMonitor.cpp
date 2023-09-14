#include "EventMonitor.hpp"

EventMonitor::EventMonitor() {
  kq = kqueue();
  quit = 0;
  assert(kq != -1);
}

EventMonitor::~EventMonitor() {
  quit = 1;
  close(kq);
}