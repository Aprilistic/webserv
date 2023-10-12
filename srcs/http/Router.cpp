#include "Router.hpp"

Router::Router()
{
}

Router::Router(const Router& other)
{
	(void)other;
}

Router& Router::operator=(const Router& other)
{
	return *this;
}

Router::~Router()
{
}

IRequestHandler* Router::Routing(Request& request)
{
	// CGI 처리
	if (IsCgiRequest(request))
	{
		return (new CgiHandler());
	}
	// 다른 요청 처리
	if (request.mMethod == "GET")
	{
		return (new GetHandler());
	}
	else if (request.mMethod == "POST")
	{
		return (new PostHandler());
	}
	else if (request.mMethod == "DELETE")
	{
		return (new DeleteHandler());
	}
	else if (request.mMethod == "PUT")
	{
		return (new PutHandler());
	}
	return (NULL);
}

bool IsCgiRequest(Request& request)
{
	// uri에  이러한 경로가 있다면 무조건  cgi에서 처리된다는 가정
	if (request.mUri.find("/cgi-bin/") != std::string::npos)
	{
		return (true);
	}
 
	// 클라이언트가 확장자를 지정해준 경우 이 파일은 무조건 cgi에서 처리된다는 가정
 	if (request.mUri.size() > 4 && request.mUri.substr(request.mUri.size() - 4) == ".bla")
    {
        return (true);
    }

	// uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

    return (false);
}

// ex
Response GetHandler::handle(Request& request) {
    Response res;
    res.mStatusCode = 200;

    res.mBody = "GET request received for URI: " + request.mUri;

    return res;
}
Response PostHandler::handle(Request& request) {
    Response res;
    res.mStatusCode = 201;

    res.mBody = "POST request received with content: " + std::string(request.mContent.begin(), request.mContent.end());

    return res;
}

Response DeleteHandler::handle(Request& request) {
    Response res;
    res.mStatusCode = 200;

    res.mBody = "DELETE request received for URI: " + request.mUri;

    return res;
}

Response PutHandler::handle(Request& request) {
    Response res;
    res.mStatusCode = 200;

    res.mBody = "PUT request received with content: " + std::string(request.mContent.begin(), request.mContent.end());

    return res;
}

Response CgiHandler::handle(Request& request) {
    Response res;
    
	if (request.mMethod == "GET")
	{

	}
	else if (request.mMethod == "POST")
	{
		
	}

    return res;
}
