#include "Router.hpp"

extern char **environ;

bool Router::IsCgiRequest(Http &request) {
  // uri에  이러한 경로가 있다면 무조건  cgi에서 처리된다는 가정
  if (request.GetRequest().mUri.find("/cgi-bin/") != std::string::npos) {
    return (true);
  }

  // 클라이언트가 확장자를 지정해준 경우 이 파일은 무조건 cgi에서 처리된다는
  // 가정
  if (request.GetRequest().mUri.size() > 4 &&
      request.GetRequest().mUri.substr(request.GetRequest().mUri.size() - 4) ==
          ".bla") {
    return (true);
  }

  // uri에 특정 location이 들어오지 않고, 확장자도 주어지지 않았는데 cgi로
  // 처리해야한다는 내용이 configfile에 들어있을 수 있는 지 확인 필요

  return (false);
}

void setAllEnv(Http &http) {
  setenv("REQUEST_METHOD", http.GetRequest().mMethod.c_str(), 1);
  setenv("CONTENT_TYPE", http.GetRequest().mContentType.c_str(), 1);
  // setenv("CONTENT_LENGTH", (http.GetRequest().mContentLength), 1);

}

eStatusCode CgiHandler::Handle(int port, Http &http) {

  int pipe_fd[2];
  if (pipe(pipe_fd) == -1) {
    // perror("pipe");
    return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR), ERROR);
  }

  pid_t pid = fork();
  if (pid == -1) {
    // perror("fork");
    return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR), ERROR);
  }

  if (pid == 0) {                    // Child Process
    close(pipe_fd[0]);               // Close read end of the pipe
    dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to write end of the pipe

    // Set environment variables
    setAllEnv(http);

    // Execute Python script
    const char *pwd = getenv("PWD");
    if (pwd == nullptr) {
      // PWD 환경 변수가 없습니다. 적절한 오류 처리를 수행합니다.
      exit(EXIT_FAILURE);
    }
    std::string path_str = std::string(pwd) + "modules/python.py";
    execve(path_str.c_str(), nullptr, environ);
    // perror("execve");
    return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR), ERROR);
  } else {             // Parent Process
    close(pipe_fd[1]); // Close write end of the pipe
    wait(nullptr);     // Wait for child process to finish

    char buffer[2048];
    ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
    if (bytes_read == -1) {
      // perror("read");
      return (http.ErrorHandle(port, SERVER_ERROR_INTERNAL_SERVER_ERROR),
              ERROR);
    }

    buffer[bytes_read] = '\0'; // Null-terminate the string
    // std::cout << "CGI Output:\n" << buffer << std::endl;
    std::string strbuffer = buffer;
    http.SetCGIbuffer(strbuffer);
  }

  return (CGI);
}