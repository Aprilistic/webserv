#include "Http.hpp"
#include "ResponseParser.hpp"
#include "Response.hpp"
#include "WebServer.hpp"

void ExitHandler(void) { system("leaks webserv"); }

int main(void) {
  Http http;
  std::string get =
      "GET / HTTP/1.1\r\n"
      "Accept: "
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
      "webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
      "Accept-Encoding: gzip, deflate, br\r\n"
      "Accept-Language: en-US,en;q=0.9,ko-KR;q=0.8,ko;q=0.7,hy;q=0.6\r\n"
      "Cache-Control: max-age=0\r\n"
      "Connection: keep-alive\r\n"
      "Host: localhost\r\n"
      "If-Modified-Since: Wed, 04 Oct 2023 01:26:01 GMT\r\n"
      "Sec-Fetch-Dest: document\r\n"
      "Sec-Fetch-Mode: navigate\r\n"
      "Sec-Fetch-Site: none\r\n"
      "Upgrade-Insecure-Requests: 1\r\n"
      "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) "
      "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 "
      "Safari/537.36\r\n"
      "\r\n";

  std::string post = "POST /login HTTP/1.1\r\n"
                    "Host: www.example.com\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Content-Length: 29\r\n"
                    "\r\n"
                    "username=test&password=1234";

  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: 12\r\n"
                         "\r\n"
                         "Hello world!";
  http.httpProcess(response);
}
