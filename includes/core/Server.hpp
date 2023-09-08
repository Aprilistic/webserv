#ifndef SERVER_HPP
#define SERVER_HPP

class Node;
class Server
{
	public:
		Server(Node* mConfigTree);
		~Server();
	private:
		// http, server, location context
		std::map<int, std::string> mErrorPage; // ex) error_page <error_code> <path> -> error_page 404 500 /path/error.html; 이런값 받으면 자료형 바꿔야함 , default : x
 		int mClientMaxBodySize; // default : 1m(1메가 바이트)
		std::vector<std::string> mIndex; // default : index.html
		bool mbAutoIndex; // on(1) / off(0) default : off

		// server context
		int mListen; // default : 80
		std::string mServerName; // default : default_server
		
		// server, location context
		std::map<int, std::string> mReturn; // default : x
		
		// location context
		std::string mAlias; // default : x
		std::vector<std::string> mLimitExcept; // default : x
};

#endif