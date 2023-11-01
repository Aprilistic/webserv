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

void setAllEnv(int port, Http &http) {
  Request tmp = http.GetRequest();
  std::stringstream ss;

  //SERVER_PROTOCOL: 클라이언트의 HTTP 프로토콜 버전.
  setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);

  // REQUEST_METHOD: 클라이언트의 요청 방식(GET, POST, HEAD 등).
  setenv("REQUEST_METHOD", tmp.mMethod.c_str(), 1);

  // QUERY_STRING: URL에서 '?' 뒤에 오는 쿼리 문자열.
  std::size_t pos = tmp.mUri.find("?");
  if (pos != std::string::npos) {
    const char *query = &tmp.mUri[pos + 1];
    setenv("QUERY_STRING", query, 1);
  } else {
    setenv("QUERY_STRING", "", 1);
  }

  // CONTENT_TYPE: 요청 본문의 MIME 타입, 주로 POST 요청에서 사용됩니다.
  setenv("CONTENT_TYPE", tmp.mContentType.c_str(), 1);

  // CONTENT_LENGTH: 요청 본문의 길이.
  ss << http.GetRequest().mContentLength;
  setenv("CONTENT_LENGTH", ss.str().c_str(), 1);

  // SCRIPT_NAME: 실행되는 CGI 스크립트의 이름.
  const char *pwd = getenv("PWD");
  std::string path_str = std::string(pwd) + "/cgi_tester";
  setenv("SCRIPT_NAME", path_str.c_str(), 1);

  // REQUEST_URI: 클라이언트가 요청한 URI.
  setenv("REQUEST_URI", tmp.mUri.c_str(), 1);
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
    setAllEnv(port, http);

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


    buffer[bytes_read] = '\0'; // Null-terminate the string
    // std::cout << "CGI Output:\n" << buffer << std::endl;
    std::cout << PURPLE << "CGI Output:\n" << buffer << RESET << std::endl;

    std::string message = &buffer[0];
    
    ssize_t bytesSent = send(socket, message.c_str(), message.size(), 0);
  }
}