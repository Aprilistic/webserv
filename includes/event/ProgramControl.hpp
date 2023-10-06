#ifndef PROGRAMCONTROL_HPP
#define PROGRAMCONTROL_HPP

#include "Core.hpp"
#include "Common.hpp"
#include "IEventHandler.hpp"

class ProgramControl : public IEventHandler {
public:
	virtual void EventHandler(struct kevent &currentEvent);
	void RegisterTerminationSignals(void);
	void UnregisterTerminationSignals(void);

private:
	static void signalHandler(int signal);
};


#endif