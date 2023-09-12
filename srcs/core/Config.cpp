#include "Config.hpp"

Config::Config(const std::string &path,
               std::map<int, std::vector<Server *> > mServersMap): mIsGood(true) {
  SetConfig(path);

  mConfigTree->PrintTree(0);
  mConfigTree->checkSum(0);

  // tokenize
  // createTree

  // validate or checksum or ...

  // CreatServer(mServersMap);
}

void Config::CreateServer(std::map<int, std::vector<Server *> > mServersMap) {
  int serverCount = 4; // mConfigTree -> n

  for (int i = 0; i < serverCount; i++) {
    mServersMap[i].push_back(new Server(mConfigTree));
  }
}

Config::~Config(void) {
  // deleteTree(mConfigTree);
}

void Config::SetConfig(const std::string &path) {
  Token token(path);
  if (token.mIsGood == false) {
    configError("Error: Failed to tokenize.");
  }
  mTokens = token.mTokens;
  createTree();
  if (mConfigTree->mIsGood == false){
    configError("Error: Failed to make Node.");
  }
  // printTree();
}

void Config::createTree(void) {
  std::vector<std::string>::iterator token = mTokens.begin();
  mConfigTree = new Node(mTokens, token, NULL, 1);
}

void Config::printTree(void) { mConfigTree->PrintTree(0); }

// void Config::deleteTree(void) { delete mConfigTree; }

// void CheckErrorPage(std::vector<std::string> &value) {
//   if (value.size() < 2) {
//     std::cerr << "Error: Insufficient arguments provided. Please provide at "
//                  "least two arguments."
//               << std::endl;
//       // return or ~
//   }

//   for (std::vector<std::string>::iterator it = value.begin();
//        it != value.end() - 1; it++) {
//     for (std::string::iterator strIt = it->begin(); strIt != it->end();
//          ++strIt) {
//       if (isdigit(*strIt) == false) {
//         std::cerr << "Error: Invalid character found in status code argument."
//                   << std::endl;
//       // return or ~
//       }
//     }
//     int statusCode = std::atoi(it->c_str());
//     if (statusCode < 300 || statusCode > 599) {
//       std::cerr << "Error: Status code out of valid range (300-599)."
//                 << std::endl;
//       // return or ~
//     }
//   }
//   // 마지막값은 유효성 검사하지 않아도 됨
// }

// void CheckClientMaxBodySize(std::vector<std::string> &value) {
//   if (value.size() != 1) {
//     std::cerr << "The number of values is not exactly 1!" << std::endl;
//       // return or ~
//   }
//   int multiplier;
//   char lastChar = value[0].back();
//   lastChar = toupper(lastChar);

//   switch (lastChar) {
//   case 'K':
//     multiplier = 1024; // KB
//     break;
//   case 'M':
//     multiplier = (1024 * 1024); // MB
//     break;
//   case 'G':
//     multiplier = 1024 * 1024 * 1024; // GB
//     break;
//   default:
//     multiplier = 1;
//     break;
//   }

//   std::string str;

//   if (multiplier == 1) {
//     str = value[0];
//   } else {
//     str = value[0].substr(0, value[0].size() - 1);
//   }

//   for (int i = 0; i < str.size(); i++) {
//     if (isdigit(str[i] == false)) {
//       std::cerr << "Invalid" << std::endl;
//       // return or ~
//     }
//   }

//   int num = atoi(value[0].c_str());

//   if (num < 0) {
//     std::cerr << "Only positive numbers are possible" << std::endl;
//       // return or ~
//   }
//   // mClientMaxBodySize = (num * multiplier);
// }

// void CheckIndex(std::vector<std::string> &value) {
//   if (value.size() < 1) {
//     std::cerr
//         << "Error: No arguments provided. At least one argument is required."
//         << std::endl;
//       // return or ~
//   }
// }

// void CheckAutoIndex(std::vector<std::string> &value) {

//   if (value.size() != 1) {
//     std::cerr << "The number of values is not exactly 1!" << std::endl;
//       // return or ~
//   }
//   std::string upperName = value[0];
//   std::transform(upperName.begin(), upperName.end(), upperName.begin(),
//                  ::toupper);
//   if (value[0] != "ON" && value[0] != "OFF") {
//     std::cerr << "The value is not ON or OFF!" << std::endl;
// 	exit(1);
//       // return or ~
//   }
// }

// void CheckListen(std::vector<std::string> &value) {
//   if (value.size() < 1)
//   {
//     std::cerr << "Error: No arguments provided. At least one argument is required." << std::endl;
//       // return or ~
//   }
// 	size_t pos = value[0].find(':');
// 	if (pos == std::string::npos)
// 	{
// 		// only Port
// 		for (int i = 0; i < value[0].size(); i++)
// 		{
//     		if (isdigit(value[0][i] == false))
// 			{
//      	 		std::cerr << "Invalid" << std::endl;
//       			// return or ~
//     		}
// 		}
// 		int port = atoi(value[0].c_str());
// 		if (port < 0 || 65535 < port)
// 		{
// 			std::cerr << "Error: 0 ~ 65535" << std::endl;
// 		}
// 	}
// 	else
// 	{
// 		// IP:Port 127.0.0.1:8080
// 		std::string ip = value[0].substr(0, pos); // ex 127.0.0.1
// 		std::string portStr = value[0].substr(pos + 1); // 8080
// 		int port = atoi(portStr.c_str()); //8080

// 		// IP validate
// 		int num;
// 		char dot;
// 		std::istringstream iss(ip);
	
// 		for (int i = 0; i < 3; i++)
// 		{
// 			iss >> num >> dot;
// 			if (num < 0 || 255 < num || dot != '.')
// 			{
// 				std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
// 			    // return or ~
// 			}
// 		}
// 		iss >> num;
// 		if (num < 0 || 255 < num || iss.eof() == false)
// 		{
// 			std::cerr << "Error: 0 ~ 255 & only ." << std::endl;
// 			// return or ~
// 		}
// 	}
// }

// void CheckServerName(std::vector<std::string> &value)
// {
//   if (value.size() < 1) {
//     std::cerr
//         << "Error: No arguments provided. At least one argument is required."
//         << std::endl;
//       // return or ~
//   }
// }


// void CheckReturn(std::vector<std::string> &value)
// {
// 	if (value.size() != 2)
// 	{
//     	std::cerr << "Error: Incorrect number of arguments. Exactly two arguments are required." << std::endl;
//      	 // return or ~
// 	}
//     std::string firstValue = value[0];

//     for (int i = 0; i < firstValue.size(); i++)
//     {
//         if (isdigit(firstValue[i]) == false)
//         {
//         	std::cerr << "Error: Invalid character found in status code argument." << std::endl;
//       		// return or ~
//         }
//     }

// 	int num = atoi(firstValue.c_str());
// 	if (num < 0 || num > 999)
// 	{
// 		std::cerr << "Error: The first value must be a number within the range 0-999." << std::endl;
// 	    // return or ~
// 	}
// }


// void CheckAlias(void)
// void CheckLimitExcept(void)

void Config::configError(const std::string &msg) {
  std::cerr << msg << std::endl;
  mIsGood = false;
}


// void Config::checkSum(int level)
// {
// //   if (level > 2)
// //     return 	;

//   for (std::map<std::string, std::vector<std::string> >::iterator it =
//            mConfigTree->mDirectives.begin();
//        it != mConfigTree->mDirectives.end(); ++it) {
//      std::cout << it->first << std::endl;
//     // if (it->first == "error_page")
//     //   CheckErrorPage(it->second);
//     // else if (it->first == "client_max_body_size")
//     //   CheckClientMaxBodySize(it->second);
//     // else if (it->first == "index")
//     //   CheckIndex(it->second);
//     // else if (it->first == "autoindex")
//     //   CheckAutoIndex(it->second);
//     // else if (it->first == "listen")
//     //   CheckListen(it->second);
//     // else if (it->first == "server_name")
//     //   CheckServerName(it->second);
//     // else if (it->first == "return")
//     //   CheckReturn(it->second);
//     // else if (it->first == "alias")
//     //   CheckAlias();
//     // else if (it->first == "limit_except")
//     //   CheckLimitExcept();
//   }
//   for (std::vector<Node *>::iterator it = mConfigTree->mChildren.begin();
//        it != mConfigTree->mChildren.end(); ++it) {
//     checkSum(level + 1);
//        }
// }