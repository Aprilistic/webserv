#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <IEventHandler.hpp>

class Signal : public IEventHandler {
public:
	Signal();
	~Signal();

	virtual void EventHandler(struct kevent &currentEvent);
};


#endif