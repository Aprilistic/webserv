#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

class Request;
class Response;

class IRequestHandler
{
	public:
		virtual Response handle(Request& request) = 0;
		virtual ~IRequestHandler() {};
};

#endif