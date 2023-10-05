#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Core.hpp"
#include "Common.hpp"
class IRequestHandler;
class Request;

class Router
{
	public:
		Router();
		~Router();
		IRequestHandler* GetHandler(Request& request);
	private:
		std::map<std::string, IRequestHandler*> mRouteTable;
};


/* test */
class StaticFileHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};

class CgiHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};
/* test */
#endif