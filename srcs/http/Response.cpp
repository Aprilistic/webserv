#include "Response.hpp"

Response::Response(Request &req, int status) : mReq(req), mStatus(status) {
  makeResponse();
}

Response::~Response() {}

std::string Response::getResponse() const { return mResponse; }

void Response::makeResponse() {
  makeResponseVersion();
  makeResponseStatusCode();
  makeResponseServer();
  makeResponseDate();
  makeResponseContentLength();
  makeResponseContentType();
  makeResponseLastModified();
  makeResponseConnection();
  makeResponseBody();
}

void Response::makeResponseVersion() {
  mResponse += "HTTP/1.1 ";
  mResponse += std::to_string(mStatus);
  mResponse += " ";
}

void Response::makeResponseStatusCode() {
  switch (mStatus) {
  case 100:
    mResponse += "Continue";
    break;
  case 101:
    mResponse += "Switching Protocols";
    break;
  case 200:
    mResponse += "OK";
    break;
  case 201:
    mResponse += "Created";
    break;
  case 202:
    mResponse += "Accepted";
    break;
  case 203:
    mResponse += "Non-Authoritative Information";
    break;
  case 204:
    mResponse += "No Content";
    break;
  case 205:
    mResponse += "Reset Content";
    break;
  case 206:
    mResponse += "Partial Content";
    break;
  case 300:
    mResponse += "Multiple Choices";
    break;
  case 301:
    mResponse += "Moved Permanently";
    break;
  case 302:
    mResponse += "Found";
    break;
  case 303:
    mResponse += "See Other";
    break;
  case 304:
    mResponse += "Not Modified";
    break;
  case 305:
    mResponse += "Use Proxy";
    break;
  case 306:
    mResponse += "(Unused)";
    break;
  case 307:
    mResponse += "Temporary Redirect";
    break;
  case 308:
    mResponse += "Permanent Redirect";
    break;
  case 400:
    mResponse += "Bad Request";
    break;
  case 401:
    mResponse += "Unauthorized";
    break;
  case 402:
    mResponse += "Payment Required";
    break;
  case 403:
    mResponse += "Forbidden";
    break;
  case 404:
    mResponse += "Not Found";
    break;
  case 405:
    mResponse += "Method Not Allowed";
    break;
  case 406:
    mResponse += "Not Acceptable";
    break;
  case 407:
    mResponse += "Proxy Authentication Required";
    break;
  case 408:
    mResponse += "Request Timeout";
    break;
  case 409:
    mResponse += "Conflict";
    break;
  case 410:
    mResponse += "Gone";
    break;
  case 411:
    mResponse += "Length Required";
    break;
  case 412:
    mResponse += "Precondition Failed";
    break;
  case 413:
    mResponse += "Payload Too Large";
    break;
  case 414:
    mResponse += "URI Too Long";
    break;
  case 415:
    mResponse += "Unsupported Media Type";
    break;
  case 416:
    mResponse += "Range Not Satisfiable";
    break;
  case 417:
    mResponse += "Expectation Failed";
    break;
  case 418:
    mResponse += "I'm a teapot";
    break;
  case 421:
    mResponse += "Misdirected Request";
    break;
  case 422:
    mResponse += "Unprocessable Entity";
    break;
  case 426:
    mResponse += "Upgrade Required";
    break;
  case 500:
    mResponse += "Internal Server Error";
    break;
  case 501:
    mResponse += "Not Implemented";
    break;
  case 502:
    mResponse += "Bad Gateway";
    break;
  case 503:
    mResponse += "Service Unavailable";
    break;
  case 504:
    mResponse += "Gateway Timeout";
    break;
  case 505:
    mResponse += "HTTP Version Not Supported";
    break;
  }
  mResponse += "\r\n";
}

void Response::makeResponseServer() {
  mResponse += "Server: ";
  mResponse += "webserv";
  mResponse += "\r\n";
}

void Response::makeResponseDate() {
  mResponse += "Date: ";
  // mResponse += "Tue, 15 Nov 1994 08:12:31 GMT";
  mResponse += "\r\n";
}

void Response::makeResponseContentLength() {
  mResponse += "Content-Length: ";
  // mResponse += std::to_string(mReq.getBody().size());
  mResponse += "\r\n";
}

void Response::makeResponseContentType() {
  mResponse += "Content-Type: ";
  // mResponse += mReq.getContentType();
  mResponse += "\r\n";
}

void Response::makeResponseLastModified() {
  mResponse += "Last-Modified: ";
  // mResponse += mReq.getLastModified();
  mResponse += "\r\n";
}

void Response::makeResponseConnection() {
  mResponse += "Connection: ";
  // mResponse += mReq.getConnection();
  mResponse += "\r\n";
}

void Response::makeResponseBody() {
  mResponse += "\r\n";
  // mResponse += mReq.getBody();
}