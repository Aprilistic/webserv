#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "Core.hpp"
#include "IEventHandler.hpp"

class Signal : public IEventHandler {
public:
	Signal(void);

	virtual void EventHandler(struct kevent &currentEvent);
	static void RegisterSignalsWithKqueue(void);
	static void UnregisterSignalsWithKqueue(void);

private:
	static void signalHandler(int signal);
	static void registerKqueueSignalHandler(int signal, void (*handler)(int));
};


#endif