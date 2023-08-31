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

int main(int, char **) {
  const char text[] =
      "GE";

  Request request;
  RequestParser parser;

  eParseResult res = parser.parse(request, text, text + sizeof(text));
  const char test[] = "T / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n";
  if (res == ParsingIncompleted)
	res = parser.parse(request, test, test + sizeof(test));

  if (res == ParsingCompleted) {
    std::cout << request.inspect() << std::endl;
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Parsing failed" << std::endl;
    return EXIT_FAILURE;
  }
}