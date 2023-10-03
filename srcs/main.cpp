#include "Http.hpp"
#include "ResponseParser.hpp"
#include "ResponseSyntax.hpp"
#include "WebServer.hpp"

void ExitHandler(void) { system("leaks webserv"); }

int main(void) {
  Http http;
  std::string str = "POST /login HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Content-Length: 26\r\n"
                    "\r\n"
                    "username=test&password=1234";

  http.httpProcess(str);
}
