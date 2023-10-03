#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

class IEventHandler {
public:
	virtual void HandleReadEvent(void) = 0;
	virtual void HandleWriteEvent(void) = 0;
	virtual void HandleTimerEvent(void) = 0;
	// virtual void HandleSignalEvent(void) = 0;
};

#endif