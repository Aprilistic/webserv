#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include "Request.hpp"
#include "IEventHandler.hpp"
#include "Enum.hpp"

class Http;

class CGI : public IEventHandler {
public:
	CGI(Http &http);
	~CGI();
	virtual void EventHandler(struct kevent &currentEvent);

	void CgiHandle();
private:
	void processHandler();
	eStatusCode cgiResponseParsing(std::string &response);
	void setAllEnv();

private:
	Http &mHttp;
	pid_t mPid;
	std::string mRequestFileName;
	std::string mOutputFileName;
};

#endif