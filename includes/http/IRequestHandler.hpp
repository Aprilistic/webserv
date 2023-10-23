#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

// class Request;
// class Response;
#include "Http.hpp"

class Http;

enum eHandleStatus {
	INCOMPLETED = 0,
	COMPLETED = 1
};

class IRequestHandler
{
	public:
			// virtual Response handle(int port, Http& http) = 0;
			// virtual Response handle( Http& http) = 0;
			virtual eStatusCode handle(int port, Http& http) = 0;
			// virtual eStatusCode handle(Http& http) = 0;
	private:
};

#endif