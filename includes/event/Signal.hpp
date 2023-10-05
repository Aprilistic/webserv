#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "Core.hpp"
#include "IEventHandler.hpp"

class Signal : public IEventHandler {
public:
	Signal(void);

	virtual void EventHandler(struct kevent &currentEvent);
	static void RegisterSignalEvent(void);
	static void UnregisterSignalEvent(void);

private:
	static void registerSignalEvent(int signal, void (*handler)(int));
	static void handleSigalEvent(void);
};


#endif