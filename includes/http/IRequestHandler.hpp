#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

// class Request;
// class Response;
#include "Http.hpp"

enum eHandleStatus {
	INCOMPLETED = 0,
	COMPLETED = 1
};

class IRequestHandler
{
	public:
		// virtual Response handle(Request& request) = 0;
			virtual Response handle(Http& http) = 0;

		// virtual eHandleStatus handle(Request& request);
	private:
};

#endif