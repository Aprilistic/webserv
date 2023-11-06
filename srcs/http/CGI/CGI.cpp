#include "CGI.hpp"
#include "Enum.hpp"

extern char **environ;

bool IsCgiRequest(Http &http, int port) {

  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.GetRequest().mHost, http.GetRequest().mUri,
      http.GetRequest().mMethod);
  if (location == NULL) {
    return (false);
  }

  std::vector<std::string> cgi_pass = location->FindValue(location, "cgi_pass");
  if (cgi_pass.size()) {
    return (true);
  }
  return (false);
}

void setAllEnv(int port, Http &http, int socket) {

  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.GetRequest().mHost, http.GetRequest().mUri,
      http.GetRequest().mMethod);
  if (location == NULL) {
    return (http.ErrorHandle(port, CLIENT_ERROR_NOT_FOUND, socket));
  }

  std::vector<std::string> server_name =
      location->FindValue(location, "server_name");
  Request tmp = http.GetRequest();
  std::stringstream ss;

  // AUTH_TYPE: 인증 방식.
  setenv("AUTH_TYPE", "", 1);

  // CONTENT_TYPE: 요청 본문의 MIME 타입, 주로 POST 요청에서 사용됩니다.
  setenv("CONTENT_TYPE", tmp.mContentType.c_str(), 1);

  // CONTENT_LENGTH: 요청 본문의 길이.
  ss << http.GetRequest().mContentLength;
  setenv("CONTENT_LENGTH", ss.str().c_str(), 1);

  // GATEWAY_INTERFACE: CGI 스펙의 버전.
  setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);

  // REQUEST_URI
  setenv("REQUEST_URI", tmp.mUri.c_str(), 1);

  // PATH_INFO: CGI 스크립트에 전달되는 추가적인 경로 정보.
  setenv("PATH_INFO", tmp.mUri.c_str(), 1);

  // PATH_TRANSLATED: PATH_INFO에 대응하는 실제 파일 경로.

  // QUERY_STRING: URL에서 '?' 뒤에 오는 쿼리 문자열.
  std::size_t pos = tmp.mUri.find("?");
  if (pos != std::string::npos) {
    const char *query = &tmp.mUri[pos + 1];
    setenv("QUERY_STRING", query, 1);
  } else {
    setenv("QUERY_STRING", "", 1);
  }

  // REMOTE_ADDR: 클라이언트의 IP 주소.
  setenv("REMOTE_ADDR", "", 1);

  // REMOTE_HOST: 클라이언트의 호스트 이름.
  setenv("REMOTE_HOST", "", 1);

  // REMOTE_IDENT: 클라이언트의 식별 정보.
  setenv("REMOTE_IDENT", "", 1);

  // REMOTE_USER: 클라이언트의 사용자 이름.
  setenv("REMOTE_USER", "", 1);

  // REQUEST_METHOD: 클라이언트의 요청 방식(GET, POST, HEAD 등).
  setenv("REQUEST_METHOD", tmp.mMethod.c_str(), 1);

  // SCRIPT_NAME: 실행되는 CGI 스크립트의 이름.
  const char *pwd = getenv("PWD");
  std::string path_str = std::string(pwd) + "/cgi_tester";
  setenv("SCRIPT_NAME", path_str.c_str(), 1);

  // SERVER_NAME: 서버의 호스트 이름.
  setenv("SERVER_NAME", server_name[0].c_str(), 1);

  // SERVER_PORT: 서버의 포트 번호.
  ss.clear();
  ss << port;
  setenv("SERVER_PORT", ss.str().c_str(), 1);

  // SERVER_PROTOCOL: 클라이언트의 HTTP 프로토콜 버전.
  setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);

  // SERVER_SOFTWARE: 서버의 소프트웨어 이름과 버전.
  setenv("SERVER_SOFTWARE", "", 1);
}

void CGIHandle(int port, Http &http, int socket) {
  // 요청 내용을 파일에 쓰기 위한 ofstream 객체 생성
  std::ofstream requestFile("cgi_request_data.txt",
                            std::ios::out | std::ios::trunc);
  if (!requestFile.is_open()) {
    std::cerr << "Failed to open request temp file." << std::endl;
    return;
  }

  // 요청 내용을 임시 파일에 쓰기
  requestFile << http.GetRequest().mContent;
  requestFile.close(); // 파일 쓰기 완료 후 닫기

  // CGI 스크립트 결과를 받을 파일 생성
  std::string outputFileName = "cgi_output_data.txt";

  // CGI 스크립트 실행을 위한 프로세스 생성
  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "Failed to fork." << std::endl;
    return;
  } else if (pid == 0) {
    // 자식 프로세스에서 CGI 스크립트 실행
    setAllEnv(port, http, socket);

    freopen("cgi_request_data.txt", "r", stdin);
    freopen(outputFileName.c_str(), "w", stdout);

    // 실행할 스크립트 경로 가져오기
    const char *pwd = getenv("PWD");
    std::string path_str = std::string(pwd) + "/cgi_tester";
    const char *c_path_str =
        path_str.c_str(); // std::string에서 C 스타일 문자열로 변환
    char *argv[] = {const_cast<char *>("cgi_tester"), NULL};
    execve(argv[0], argv, environ);
    // execve 실패 시
    exit(EXIT_FAILURE);
  } else {
    // 부모 프로세스
    int status;
    waitpid(pid, &status, 0); // 자식 프로세스가 종료될 때까지 대기

    // CGI 스크립트의 출력을 읽기 위한 ifstream 객체 생성
    std::ifstream responseFile(outputFileName.c_str(), std::ios::in);
    if (!responseFile.is_open()) {
      std::cerr << "Failed to open response temp file." << std::endl;
      return;
    }

    // 파일 내용을 읽어 응답 객체에 저장
    std::string response((std::istreambuf_iterator<char>(responseFile)),
                         std::istreambuf_iterator<char>());
    // response parsing
    std::map<std::string, std::string> headers;
    std::string body;

    std::istringstream responseStream(response);
    std::string line;
    bool headerSection = true;

    while (std::getline(responseStream, line)) {
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (line.empty()) {
        headerSection = false;
        continue;
      }

      if (headerSection) {
        std::size_t pos = line.find(": ");
        if (pos != std::string::npos) {
          std::string key = line.substr(0, pos);
          std::string value = line.substr(pos + 2);
          headers[key] = value;
        }
      } else {
        body += line + "\n";
      }
    }
    if (!body.empty() && body.back() == '\n') {
      body.pop_back();
    }

    http.GetResponse().mBody = body;

    // 파일 닫기
    responseFile.close();

    // 임시 파일 삭제
    unlink("cgi_request_data.txt");
    unlink(outputFileName.c_str());

    // 응답 보내기
    http.SendResponse(SUCCESSFUL_OK, port, socket);
  }
}