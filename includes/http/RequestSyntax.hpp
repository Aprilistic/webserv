#ifndef REQUESTSYNTAX_HPP
#define REQUESTSYNTAX_HPP

#include "Core.hpp"
#include "Request.hpp"
#include "Enum.hpp"


class RequestSyntax {
	public:
		RequestSyntax();
		~RequestSyntax();

		eStatustCode checksyntax(Request &request);
		eStatustCode method(Request& request); //mMethod
		eStatustCode requestTarget(Request& request);// mUri
		eStatustCode httpVersion(Request& request); // mVersionMajor, mVersionMinor
		eStatustCode headerField(Request& request); // mHeaders<field-name, field-value>
		eStatustCode messageBody(Request& request); // mContent
	private:
};
#endif