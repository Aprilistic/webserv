#include "Http.hpp"
#include "ResponseParser.hpp"

void ResponseParser::setResponse(Http &http, eStatusCode state) {
  setStatusLine(http, state);
  setMandatoryHeaderFields(http);
  setCookie(http);
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
  case (REDIRECTION_MOVED_PERMANENTLY):
    return ("Moved Permanently");
  case (REDIRECTION_FOUND):
    return ("Found");
  case (REDIRECTION_SEE_OTHER):
    return ("See Other");
  case (REDIRECTION_NOT_MODIFIED):
    return ("Not Modified");
  case (REDIRECTION_USE_PROXY):
    return ("Use Proxy");
  case (REDIRECTION_TEMPORARY_REDIRECT):
    return ("Temporary Redirect");
  case (REDIRECTION_PERMANENT_REDIRECT):
    return ("Permanent Redirect");
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
  case (CLIENT_ERROR_CONTENT_TOO_LARGE):
    return ("Content Too Large");
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
  std::string filepath = http.GetResponse().GetFilename();
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
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S KST", timeinfo);
  http.GetResponse().InsertHeader("Date", buf);

  // Server
  http.GetResponse().InsertHeader("Server", "*u*king webserv");

  // Content-Length
  std::string contentLength = ToString(http.GetResponse().GetBody().size());
  http.GetResponse().InsertHeader("Content-Length", contentLength);

  // Content-Type
  if (http.GetResponse().GetHeaders().find("Content-Type") ==
          http.GetResponse().GetHeaders().end() &&
      http.GetResponse().GetBody().size() != 0) {
    std::string contentType = getFileType(http);
    http.GetResponse().InsertHeader("Content-Type", contentType);
  }

  // Connection
  if (http.GetRequest().GetKeepAlive() == true) {
    http.GetResponse().InsertHeader("Connection", "keep-alive");
  } else {
    http.GetResponse().InsertHeader("Connection", "close");
  }
}
