#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

class Request;
class Response;

enum eHandleStatus {
	INCOMPLETED = 0,
	COMPLETED = 1
};

class IRequestHandler
{
	public:
		virtual Response handle(Request& request) = 0;
		// virtual eHandleStatus handle(Request& request);
	private:
};

#endif