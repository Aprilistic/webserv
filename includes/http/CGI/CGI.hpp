#ifndef CGI_HPP
#define CGI_HPP

#include "Core.hpp"
#include "Http.hpp"
#include "IEventHandler.hpp"
#include "Request.hpp"

class Http;

// class CGI : public IEventHandler {
// public:
//   CGI(int port, Http &http, int socket);
// 	~CGI();
// 	virtual void EventHandler(struct kevent &currentEvent);

// private:
//   CGI();
//   void readHandler();
// 	void procHandler();
// 	void 
// };

bool IsCgiRequest(Request &request);
void CGIHandle(int port, Http &http, int socket);

#endif