#ifndef REQUESTSYNTAX_HPP
#define REQUESTSYNTAX_HPP

#include "Core.hpp"
#include "Request.hpp"
#include "Enum.hpp"


class RequestSyntax {
	public:
		RequestSyntax();
		~RequestSyntax();

		eStatusCode checksyntax(Request &request);
		eStatusCode method(Request& request); //mMethod
		eStatusCode requestTarget(Request& request);// mUri
		eStatusCode httpVersion(Request& request); // mVersionMajor, mVersionMinor
		eStatusCode headerField(Request& request); // mHeaders<field-name, field-value>
		eStatusCode messageBody(Request& request); // mContent
	private:
};
#endif