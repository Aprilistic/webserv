#include "Http.hpp"
#include "ResponseParser.hpp"

void ResponseParser::setResponse(Http &http, eStatusCode state) {
  setStatusLine(http, state);
  setMandatoryHeaderFields(http);
}

void ResponseParser::setStatusLine(Http &http, eStatusCode state) {
  // Server Version
  http.GetResponse().SetVersionMajor(http.GetRequest().GetVersionMajor());
  http.GetResponse().SetVersionMinor(http.GetRequest().GetVersionMinor());

  // Status Code
  http.GetResponse().SetStatusCode(state);
  http.GetResponse().SetStatus(getStatusMessage(state));
}

std::string ResponseParser::getStatusMessage(eStatusCode errorStatus) {
  switch (errorStatus) {
  case (SUCCESSFUL_OK):
    return ("OK");
  case (SUCCESSFUL_CREATERD):
    return ("Created");
  case (SUCCESSFUL_ACCEPTED):
    return ("Accepted");
  case (SUCCESSFUL_NO_CONTENT):
    return ("No Content");
  case (SUCCESSFUL_PARTIAL_CONTENT):
    return ("Partial Content");
  case (REDIRECT):
    return ("Redirect");
  case (CLIENT_ERROR_BAD_REQUEST):
    return ("Bad Request");
  case (CLIENT_ERROR_UNAUTHORIZED):
    return ("Unauthorized");
  case (CLIENT_ERROR_FORBIDDEN):
    return ("Forbidden");
  case (CLIENT_ERROR_NOT_FOUND):
    return ("Not Found");
  case (CLIENT_ERROR_METHOD_NOT_ALLOWED):
    return ("Method Not Allowed");
  case (CLIENT_ERROR_UNSUPPORTED_MEDIA_TYPE):
    return ("Unsupported Media Type");
  case (CLIENT_ERROR_RANGE_NOT_SATISFIABLE):
    return ("Range Not Satisfiable");
  case (SERVER_ERROR_INTERNAL_SERVER_ERROR):
    return ("Internal Server Error");
  case (SERVER_ERROR_NOT_IMPLEMENTED):
    return ("Not Implemented");
  case (SERVER_ERROR_BAD_GATEWAY):
    return ("Bad Gateway");
  case (SERVER_ERROR_GATEWAY_TIMEOUT):
    return ("Gateway Timeout");
  default:
    return ("Unknown");
  }
}

std::string ResponseParser::getFileType(Http &http) {
  if (http.GetRequest().mMethod == "POST") {
    return (http.GetRequest().mContentType);
  }
  std::string filepath = http.GetResponse().mFilename;
  if (filepath == "autoindex") {
    return ("text/html");
  }
  std::string fileType = filepath.substr(filepath.find_last_of(".") + 1);

  if (fileType == "css") {
    return ("text/css");
  } else if (fileType == "gif") {
    return ("image/gif");
  } else if (fileType == "htm" || fileType == "html") {
    return ("text/html");
  } else if (fileType == "ico") {
    return ("image/x-icon");
  } else if (fileType == "jpeg" || fileType == "jpg") {
    return ("image/jpeg");
  } else if (fileType == "js") {
    return ("text/javascript");
  } else if (fileType == "json") {
    return ("application/json");
  } else if (fileType == "pdf") {
    return ("application/pdf");
  } else if (fileType == "png") {
    return ("image/png");
  } else if (fileType == "svg") {
    return ("image/svg+xml");
  } else if (fileType == "txt") {
    return ("text/plain");
  } else if (fileType == "xml") {
    return ("text/xml");
  }
  return ("application/octet-stream");
}

void ResponseParser::setMandatoryHeaderFields(Http &http) {
  // Date
  time_t now = time(0);
  struct tm *timeinfo = localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  http.GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Date", buf));

  // Server
  http.GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Server", "*u*king webserv"));

  // Content-Length
  std::string contentLength = std::to_string(http.GetResponse().mBody.size());
  http.GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Content-Length", contentLength));

  // Content-Type
  if (http.GetResponse().mHeaders.find("Content-Type") ==
      http.GetResponse().mHeaders.end()) {
    std::string contentType = getFileType(http);
    http.GetResponse().mHeaders.insert(
        std::pair<std::string, std::string>("Content-Type", contentType));
  }

  // Connection
  if (http.GetRequest().mKeepAlive == true) {
    http.GetResponse().mHeaders.insert(
        std::pair<std::string, std::string>("Connection", "keep-alive"));
  } else {
    http.GetResponse().mHeaders.insert(
        std::pair<std::string, std::string>("Connection", "close"));
  }
}
