#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

class Response;

class ResponseBuilder
{
	public:
		std::vector<char> build(Response& response);
	private:
};

#endif