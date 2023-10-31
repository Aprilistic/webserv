#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"

class Http;

bool IsCgiRequest(Request &request);
void CGIHandle(int port, Http &http);

#endif