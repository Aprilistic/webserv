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

// 1. config 파일 확인
// 2. config 트리 생성
// 3. config 트리 유효성 check
// 4. i/o 관련 class 생성 (socket, 각 클라이언트에 대응하는 buffer 관리)
// kqueue와 연동
// 5. request가 들어오면 하나씩 읽으면서 syntax check (오류 시 바로 error 리턴)
// 6. request class 생성 (node를 순회하면서 request에 필요한 data 다 가져옴)
// 7. request class를 기반으로 response class 생성
// 8. response class로 넘어가면서 request class 소멸
// 9. response 보내고 response class 소멸
// 10. i/o 관련 class는 서버 종료시 삭제 (각 클라이언트에 대응하는 값들은
// 클라이언트가 종료 신호를 보내거나 시간이 오래 지나면 그 값들 제거)
