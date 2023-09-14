#include "Master.hpp"

void ExitHandler(void)
{
	system("leaks webserv");
}

int main(int argc, char** argv)
{
	// atexit(ExitHandler);
	if (argc == 1)
	{
		std::cout << "default config" << std::endl;
	}
	else if (argc == 2)
	{
		Master master(argv[1]);
	}
	else
	{
		std::cerr << "error" << std::endl;
	}
}

// 1. config 파일 확인
// 2. config 트리 생성
// 3. config 트리 유효성 check
// 4. i/o 관련 class 생성 (socket, 각 클라이언트에 대응하는 buffer 관리) kqueue와 연동
// 5. request가 들어오면 하나씩 읽으면서 syntax check (오류 시 바로 error 리턴)
// 6. request class 생성 (node를 순회하면서 request에 필요한 data 다 가져옴)
// 7. request class를 기반으로 response class 생성
// 8. response class로 넘어가면서 request class 소멸
// 9. response 보내고 response class 소멸
// 10. i/o 관련 class는 서버 종료시 삭제 (각 클라이언트에 대응하는 값들은 클라이언트가 종료 신호를 보내거나 시간이 오래 지나면 그 값들 제거)

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

// #include "Request.hpp"
// #include "RequestParser.hpp"
// #include "RequestSyntax.hpp"

// int main(int, char **) {
//   const char text[] = "GET  / HTTP/1.1\r\n";

//   Request request;
//   RequestParser parser;
//   RequestSyntax syntax;

//   eParseResult res = parser.parse(request, text, text + sizeof(text));
//   const char test[] =
//       "Host: localhost:8080\r\nUser-Agent: curl/7.64.1\r\nAccept: */*\r\n\r\n";
//   if (res == ParsingIncompleted)
//     res = parser.parse(request, test, test + sizeof(test));

//   if (res == ParsingCompleted) {
//     std::string res;
//     if (syntax.syntax(request) == SUCCESSFUL_OK) {
//       std::cout << request.inspect() << std::endl;
//     } else {
//       std::cout << syntax.syntax(request) << std::endl;
//     }
//     return EXIT_SUCCESS;
//   } else if (res == ParsingIncompleted) {
//     std::cout << "incom" << std::endl;
//   } else {
//     std::cerr << "Parsing failed" << std::endl;
//     return EXIT_FAILURE;
//   }
// }