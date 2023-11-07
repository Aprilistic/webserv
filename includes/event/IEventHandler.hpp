#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

class IEventHandler {
public:
  virtual void EventHandler(struct kevent &currentEvent) = 0;
};

#endif