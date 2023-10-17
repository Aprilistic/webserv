#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Core.hpp"
#include "Common.hpp"
#include "IRequestHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Http;
class Request;
class Router
{
	public:
		// static IRequestHandler* Routing(Request& request);
		static IRequestHandler* Routing(Http& http);
		static bool IsCgiRequest(Http& http);
	private:
		Router();
		~Router();
		Router(const Router& other);
		Router& operator=(const Router& other);
};

class GetHandler : public IRequestHandler
{
	public:
		virtual Response handle(Http& http);
};

class PostHandler : public IRequestHandler
{
	public:
		virtual Response handle(Http& http);
};

class DeleteHandler : public IRequestHandler
{
	public:
		virtual Response handle(Http& http);
};

class PutHandler : public IRequestHandler
{
	public:
		virtual Response handle(Http& http);
};
class CgiHandler : public IRequestHandler
{
	public:
		virtual Response handle(Http& http);
};


#endif