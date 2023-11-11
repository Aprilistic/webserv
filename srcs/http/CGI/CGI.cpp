#include "CGI.hpp"
#include "ConfigMap.hpp"

extern char **environ;

CGI::CGI(Http &http) : mHttp(http) {}

CGI::~CGI() {}

void CGI::EventHandler(struct kevent &currentEvent) {
  if (currentEvent.flags & EV_ERROR) {
    return ;
  }
  switch (currentEvent.filter) {
  case EVFILT_PROC:
    processHandler(currentEvent);
    break;
  default:
    break;
  }
}

void CGI::processHandler(struct kevent &currentEvent) {
  if ((currentEvent.fflags & NOTE_EXIT) == 0){
    return ;
  }

  int status;
  waitpid(mPid, &status, WNOHANG); // 자식 프로세스가 종료될 때까지 대기
  int exitStatus = 0;
  if (WIFEXITED(status)) {
    exitStatus = WEXITSTATUS(status); // 자식 프로세스의 종료 상태를 반환
  } else if (WIFSIGNALED(status)) {
    exitStatus = WTERMSIG(status);
  }
  if (exitStatus != 0) {
    return (mHttp.ErrorHandle(SERVER_ERROR_INTERNAL_SERVER_ERROR));
  }

  // CGI 스크립트의 출력을 읽기 위한 ifstream 객체 생성
  std::ifstream responseFile(mOutputFileName.c_str(), std::ios::in);
  if (!responseFile.is_open()) {
    return (mHttp.ErrorHandle(SERVER_ERROR_INTERNAL_SERVER_ERROR));
  }

  // 파일 내용을 읽어 응답 객체에 저장
  std::string response((std::istreambuf_iterator<char>(responseFile)),
                       std::istreambuf_iterator<char>());

  // response parsing
  eStatusCode statusCode = cgiResponseParsing(response);

  // 파일 닫기
  responseFile.close();

  // 임시 파일 삭제
  unlink(mRequestFileName.c_str());
  unlink(mOutputFileName.c_str());

  // 응답 보내기

  mHttp.SendResponse(statusCode);
}

void CGI::setAllEnv() {

  Node *location = Common::mConfigMap->GetConfigNode(
      mHttp.GetPort(), mHttp.GetRequest().GetHost(),
      mHttp.GetRequest().GetUri(), mHttp.GetRequest().GetMethod());
  if (location == NULL) {
    return (mHttp.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
  }

  std::vector<std::string> server_name =
      location->FindValue(location, "server_name");
  Request tmp = mHttp.GetRequest();
  std::stringstream ss;

  // AUTH_TYPE: 인증 방식.
  setenv("AUTH_TYPE", "", 1);

  // CONTENT_TYPE: 요청 본문의 MIME 타입, 주로 POST 요청에서 사용됩니다.
  setenv("CONTENT_TYPE", tmp.GetContentType().c_str(), 1);

  // CONTENT_LENGTH: 요청 본문의 길이.
  ss << mHttp.GetRequest().GetContentLength();
  setenv("CONTENT_LENGTH", ss.str().c_str(), 1);

  // GATEWAY_INTERFACE: CGI 스펙의 버전.
  setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);

  // REQUEST_URI
  setenv("REQUEST_URI", tmp.GetUri().c_str(), 1);

  // PATH_INFO: CGI 스크립트에 전달되는 추가적인 경로 정보.
  setenv("PATH_INFO", tmp.GetUri().c_str(), 1);

  // PATH_TRANSLATED: PATH_INFO에 대응하는 실제 파일 경로.

  // QUERY_STRING: URL에서 '?' 뒤에 오는 쿼리 문자열.
  std::size_t pos = tmp.GetUri().find("?");
  if (pos != std::string::npos) {
    std::string uri = tmp.GetUri();
    const char *query = uri.c_str() + pos + 1;
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
  setenv("REQUEST_METHOD", tmp.GetMethod().c_str(), 1);

  // SCRIPT_NAME: 실행되는 CGI 스크립트의 이름.
  const char *pwd = getenv("PWD");
  std::string path_str = std::string(pwd) + "/cgi_tester";
  setenv("SCRIPT_NAME", path_str.c_str(), 1);

  // SERVER_NAME: 서버의 호스트 이름.
  setenv("SERVER_NAME", server_name[0].c_str(), 1);

  // SERVER_PORT: 서버의 포트 번호.
  ss.clear();
  ss << mHttp.GetPort();
  setenv("SERVER_PORT", ss.str().c_str(), 1);

  // SERVER_PROTOCOL: 클라이언트의 HTTP 프로토콜 버전.
  setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);

  // SERVER_SOFTWARE: 서버의 소프트웨어 이름과 버전.
  setenv("SERVER_SOFTWARE", "", 1);

  std::multimap<std::string, std::string> headers = tmp.GetHeaders();
  for (std::multimap<std::string, std::string>::iterator it = headers.begin();
       it != headers.end(); ++it) {
    std::string key = it->first;
    std::string value = it->second;

    // 헤더 이름에 -가 있으면 _로 변경
    std::replace(key.begin(), key.end(), '-', '_');

    // 헤더 이름을 대문자로 변경
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);

    if (getenv(key.c_str()) == NULL) {
      std::string httpEnvKey = "HTTP_" + key;
      setenv(httpEnvKey.c_str(), value.c_str(), 1);
    }
  }
}

eStatusCode CGI::cgiResponseParsing(std::string &response) {
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

  // cgi 헤더에서 status code 가져오기
  std::string statusHeader = headers["Status"];
  statusHeader = statusHeader.substr(0, statusHeader.find(" "));

  eStatusCode statusCode =
      static_cast<eStatusCode>(std::atoi(statusHeader.c_str()));

  // headers에서 status 헤더 제거
  headers.erase("Status");

  // response에 cgi 헤더 추가
  for (std::map<std::string, std::string>::iterator it = headers.begin();
       it != headers.end(); ++it) {
    mHttp.GetResponse().InsertHeader(it->first, it->second);
  }

  mHttp.GetResponse().SetBody(body);

  return (statusCode);
}

void CGI::CgiHandle() {
  static int cnt;
  // 요청 내용을 파일에 쓰기 위한 ofstream 객체 생성
  std::string tmpFileName = "cgi_request_" + toString(cnt);
  mRequestFileName = "./tmp/" + tmpFileName + ".txt";
  std::ofstream requestFile(mRequestFileName.c_str(),
                            std::ios::out | std::ios::trunc);
  if (!requestFile.is_open()) {
    std::cerr << "Failed to open request temp file." << std::endl;
    return (mHttp.ErrorHandle(SERVER_ERROR_INTERNAL_SERVER_ERROR));
  }

  // 요청 내용을 임시 파일에 쓰기
  requestFile << mHttp.GetRequest().GetContent();
  requestFile.close(); // 파일 쓰기 완료 후 닫기

  // CGI 스크립트 결과를 받을 파일 생성
  tmpFileName = "cgi_output_" + toString(cnt++);
  mOutputFileName = "./tmp/" + tmpFileName + ".txt";

  // CGI 스크립트 실행을 위한 프로세스 생성
  mPid = fork();
  if (mPid == -1) {
    return (mHttp.ErrorHandle(SERVER_ERROR_INTERNAL_SERVER_ERROR));
  } else if (mPid == 0) {
    // 자식 프로세스에서 CGI 스크립트 실행
    setAllEnv();

    freopen(mRequestFileName.c_str(), "r", stdin);
    freopen(mOutputFileName.c_str(), "w", stdout);

    // cgi pass 가져오기
    Node *location = Common::mConfigMap->GetConfigNode(
        mHttp.GetPort(), mHttp.GetRequest().GetHost(),
        mHttp.GetRequest().GetUri(), mHttp.GetRequest().GetMethod());
    if (location == NULL) {
      return (mHttp.ErrorHandle(CLIENT_ERROR_NOT_FOUND));
    }
    std::string cgiPass = location->FindValue(location, "cgi_pass")[0];
    execve(cgiPass.c_str(), NULL, environ);
    // execve 실패 시
    exit(EXIT_FAILURE);
  } else {
    // 부모 프로세스
    fcntl(mPid, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    struct kevent event;
    EV_SET(&event, mPid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0, this);
    kevent(Common::mKqueue, &event, 1, NULL, 0, NULL);
  }
}
