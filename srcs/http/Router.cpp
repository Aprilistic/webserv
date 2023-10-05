#include "Router.hpp"

// Router::Router()
// {
// 	mRouteTable["/static"] = new StaticFileHandler();
// 	mRouteTable["/cgi"] = new CgiHandler();
// }

// Router::~Router()
// {
// 	std::map<std::string, IHandler*>::iterator it;
// 	for (it = mRouteTable.begin(); it != mRouteTable.end(); ++it) {
// 		delete it->second;
// 	}
// }

// IRequestHandler* Router::GetHandler(Request& request)
// {
// 	// find handler
// }