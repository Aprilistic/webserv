#include "Config.hpp"

// int main(int argc, char** argv)
// {
// 	if (argc != 2)
// 	{
// 		std::cerr << "Error: Invalid arguments" << std::endl;
// 		return (1);
// 	}
// 	Config config(argv[1]);
// 	return (0);
// }

#include "Request.hpp"
#include "RequestParser.hpp"
#include "RequestSyntax.hpp"

int main(int, char **) {
  const char text[] = "GET  / HTTP/1.1\r\n";

  Request request;
  RequestParser parser;
  RequestSyntax syntax;

  eParseResult res = parser.parse(request, text, text + sizeof(text));
  const char test[] =
      "Host: localhost:8080\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n";
  if (res == ParsingIncompleted)
    res = parser.parse(request, test, test + sizeof(test));

  if (res == ParsingCompleted) {
    std::string res;
    if (syntax.syntax(request) == SUCCESSFUL_OK) {
      std::cout << request.inspect() << std::endl;
    } else {
      std::cout << syntax.syntax(request) << std::endl;
    }
    return EXIT_SUCCESS;
  } else if (res == ParsingIncompleted) {
    std::cout << "incom" << std::endl;
  } else {
    std::cerr << "Parsing failed" << std::endl;
    return EXIT_FAILURE;
  }
}