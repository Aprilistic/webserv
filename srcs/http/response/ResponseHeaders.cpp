#include "Http.hpp"

std::string Http::GetFileType() {
  if (GetRequest().mMethod == "POST") {
    return (GetRequest().mContentType);
  } else if (GetRequest().mMethod == "PUT" && GetResponse().mBody.size() != 0) {
    return (GetRequest().mContentType);
  }
  std::string filepath = GetResponse().mFilename;
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

void Http::MakeMandatoryHeaders() {
  // Server Version
  GetResponse().mVersionMajor = GetRequest().mVersionMajor;
  GetResponse().mVersionMinor = GetRequest().mVersionMinor;

  // Date
  time_t now = time(0);
  struct tm *timeinfo = localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
  GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Date", buf));

  // Server
  GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Server", "*u*king webserv"));

  // Content-Length
  std::string contentLength = std::to_string(GetResponse().mBody.size());
  GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Content-Length", contentLength));

  // Content-Type
  std::string contentType = GetFileType();
  GetResponse().mHeaders.insert(
      std::pair<std::string, std::string>("Content-Type", contentType));

  // Connection
  if (GetRequest().mKeepAlive == true) {
    GetResponse().mHeaders.insert(
        std::pair<std::string, std::string>("Connection", "keep-alive"));
  } else {
    GetResponse().mHeaders.insert(
        std::pair<std::string, std::string>("Connection", "close"));
  }
}