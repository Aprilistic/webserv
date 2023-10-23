#include "Http.hpp"

std::string Http::getFileType() {
  if (getRequest().mMethod == "POST") {
    return (getRequest().mContentType);
  } else if (getRequest().mMethod == "PUT" && getResponse().mBody.size() != 0) {
    return (getRequest().mContentType);
  }
  std::string filepath = getResponse().mFilename;
  if (filepath == "autoindex") {
    return ("text/html");
  }
  std::string fileType = filepath.substr(filepath.find_last_of(".") + 1);

  if (fileType == "bla") {
    return ("text/plain");
  } else if (fileType == "css") {
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

void Http::MakeMandatoryHeaders() {
  // Server Version
  getResponse().mVersionMajor = getRequest().mVersionMajor;
  getResponse().mVersionMinor = getRequest().mVersionMinor;

  // Date
  time_t now = time(0);
  struct tm *timeinfo = localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  getResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Date", buf));

  // Server
  getResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Server", "*u*king webserv"));

  // Content-Length
  std::string contentLength = std::to_string(getResponse().mBody.size());
  getResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Content-Length", contentLength));

  // Content-Type
  std::string contentType = getFileType();
  getResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Content-Type", contentType));

  // Connection
  if (getRequest().mHeaders.find("Connection") != getRequest().mHeaders.end()) {
    getResponse().mHeaders.insert(std::pair<std::string, std::string>(
        "Connection", getRequest().mHeaders.find("Connection")->second));
  } else {
    getResponse().mHeaders.insert(
        std::pair<std::string, std::string>("Connection", "close"));
  }
}