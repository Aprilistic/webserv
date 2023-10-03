#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

class IEventHandler {
public:
	virtual void HandleReadEvent(struct kevent& currentEvent) = 0;
	virtual void HandleWriteEvent(struct kevent& currentEvent) = 0;
	virtual void HandleTimerEvent(struct kevent& currentEvent) = 0;
	virtual void HandleSignalEvent(struct kevent& currentEvent) = 0;
};

#endif