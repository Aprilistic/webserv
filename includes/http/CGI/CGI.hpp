#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include "Request.hpp"

class Http;

bool IsCgiRequest(Http &http, int port);
void CGIHandle(int port, Http &http, int socket);

#endif