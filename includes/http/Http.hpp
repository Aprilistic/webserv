#ifndef HTTP_HPP
#define HTTP_HPP

#include "Core.hpp"
#include "Request.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"

class Http {
	public:
		Http();
		~Http();
		void waitRequest(std::string &buf);
		void makeOneRequest(std::string &buf);
		int	getStatus() const;
	private:
		Request mReq; 
		eParseResult mRes;
		eStatustCode mStatus;
};

#endif