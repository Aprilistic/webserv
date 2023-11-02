#include "CGI.hpp"

extern char **environ;

bool IsCgiRequest(Request &request) {
  // uri에  이러한 경로가 있다면 무조건  cgi에서 처리된다는 가정
  if (request.mUri.find("/cgi-bin/") != std::string::npos) {
    return (true);
  }

  // 클라이언트가 확장자를 지정해준 경우 이 파일은 무조건 cgi에서 처리된다는
  // 가정
  if (request.mUri.size() > 4 &&
      request.mUri.substr(request.mUri.size() - 4) == ".bla") {
    return (true);
  }

  // uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로
  // 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

  return (false);
}

void setAllEnv(int port, Http &http, int socket) {
  Node *location = Common::mConfigMap->GetConfigNode(
      port, http.GetRequest().mHost, http.GetRequest().mUri);
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

  // PATH_INFO: CGI 스크립트에 전달되는 추가적인 경로 정보.
  std::size_t pos = http.GetRequest().mUri.find(".bla");
  std::cout << RED << http.GetRequest().mUri.substr(pos + 4).c_str() << RESET << std::endl;

  if (pos == std::string::npos) {
    setenv("PATH_INFO", "", 1);
  }
  else  {
    setenv("PATH_INFO", http.GetRequest().mUri.substr(pos + 4).c_str(), 1);
  }

  // PATH_TRANSLATED: PATH_INFO에 대응하는 실제 파일 경로.

  // QUERY_STRING: URL에서 '?' 뒤에 오는 쿼리 문자열.
  pos = tmp.mUri.find("?");
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

  // SERVER_PROTOCOL: 클라이언트의 HTTP 프로토콜 버전.
  setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
}

void CGIHandle(int port, Http &http, int socket) {

  int pipe_fd[2];
  if (pipe(pipe_fd) == -1) {
    // perror("pipe");
    return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR, socket));
  }

  pid_t pid = fork();
  if (pid == -1) {
    // perror("fork");
    return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR, socket));
  }

  if (pid == 0) {                    // Child Process
    close(pipe_fd[0]);               // Close read end of the pipe
    dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to write end of the pipe

    // Set environment variables
    setAllEnv(port, http, socket);

    // Execute Python script
    const char *pwd = getenv("PWD");
    if (pwd == nullptr) {
      // PWD 환경 변수가 없습니다. 적절한 오류 처리를 수행합니다.
      exit(EXIT_FAILURE);
    }
    std::string path_str = std::string(pwd) + "/cgi_tester";
    execve(path_str.c_str(), nullptr, environ);
    // perror("execve");
    return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR, socket));
  } else {             // Parent Process
    close(pipe_fd[1]); // Close write end of the pipe
    int status;
    waitpid(pid, &status, 0); // Wait for child process to finish

    char buffer[10000];
    ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
      // perror("read");
      return (
          http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR, socket));
    }

    std::cout << PURPLE << "CGI Output:\n" << std::endl;
    std::cout << PURPLE << buffer << RESET << std::endl;

    std::string message = &buffer[0];

    ssize_t bytesSent = send(socket, message.c_str(), message.size(), 0);
  }
}