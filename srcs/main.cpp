#include "WebServer.hpp"

void ExitHandler(void) { system("leaks webserv"); }

int main(int argc, char **argv) {
  // atexit(ExitHandler);
  // if (argc == 1) {
  //   std::cout << "default config" << std::endl;
  // } else if (argc == 2) {

  // WebServer WebServer(argv[1]);
  // WebServer.Run();
  // } else {
  //   std::cout << "Usage: ./" << argv[0] << "[config_file]" << std::endl;
  // }
  // return (0);
  std::string string = "GET HTTP/1.1\r\n"
                       "Host: localhost:4242\r\n"
                       "User-Agent: curl/7.64.1\r\n"
                       "Accept: */*\r\n\r\n";

  Request request;
  RequestParser parser;
  std::cout << parser.parse(request, string.c_str(), string.c_str() + string.size()) << std::endl;

  std::cout << request.inspect() << std::endl;
  return 0;
}