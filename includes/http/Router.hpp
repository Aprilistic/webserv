#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Core.hpp"
#include "Common.hpp"
#include "IRequestHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"

// class IRequestHandler;
class Request;

class Router
{
	public:
		Router();
		~Router();
		Router(const Router& other);
		Router& operator=(const Router& other);
		
		IRequestHandler* Routing(Request& request);
		bool IsCgiRequest(Request& request);
	private:
};

class GetHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};

class PostHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};

class DeleteHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};

class PutHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};
class CgiHandler : public IRequestHandler
{
	public:
		virtual Response handle(Request& request);
};


#endif