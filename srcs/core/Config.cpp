#include "Config.hpp"

Config::Config(const std::string &path,
               std::map<int, std::vector<Server *> > mServersMap) {
  SetConfig(path);
  printTree();
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
  mTokens = token.mTokens;
  createTree();
  // printTree();
}

void Config::createTree(void) {
  std::vector<std::string>::iterator token = mTokens.begin();
  mConfigTree = new Node(mTokens, token, NULL, 1);
}

void Config::printTree(void) { mConfigTree->PrintTree(0); }

// void Config::deleteTree(void) { delete mConfigTree; }

void CheckErrorPage(std::vector<std::string> &value) {
  if (value.size() < 2) {
    std::cerr << "Error: Insufficient arguments provided. Please provide at "
                 "least two arguments."
              << std::endl;
      // return or ~
  }

  for (std::vector<std::string>::iterator it = value.begin();
       it != value.end() - 1; it++) {
    for (std::string::iterator strIt = it->begin(); strIt != it->end();
         ++strIt) {
      if (isdigit(*strIt) == false) {
        std::cerr << "Error: Invalid character found in status code argument."
                  << std::endl;
      // return or ~
      }
    }
    int statusCode = std::atoi(it->c_str());
    if (statusCode < 300 || statusCode > 599) {
      std::cerr << "Error: Status code out of valid range (300-599)."
                << std::endl;
      // return or ~
    }
  }
  // 마지막값은 유효성 검사하지 않아도 됨
}

void CheckClientMaxBodySize(std::vector<std::string> &value) {
  if (value.size() != 1) {
    std::cerr << "The number of values is not exactly 1!" << std::endl;
      // return or ~
  }
  int multiplier;
  char lastChar = value[0].back();
  lastChar = toupper(lastChar);

  switch (lastChar) {
  case 'K':
    multiplier = 1024; // KB
    break;
  case 'M':
    multiplier = (1024 * 1024); // MB
    break;
  case 'G':
    multiplier = 1024 * 1024 * 1024; // GB
    break;
  default:
    multiplier = 1;
    break;
  }

  std::string str;

  if (multiplier == 1) {
    str = value[0];
  } else {
    str = value[0].substr(0, value[0].size() - 1);
  }

  for (int i = 0; i < str.size(); i++) {
    if (isdigit(str[i] == false)) {
      std::cerr << "Invalid" << std::endl;
      // return or ~
    }
  }

  int num = atoi(value[0].c_str());

  if (num < 0) {
    std::cerr << "Only positive numbers are possible" << std::endl;
      // return or ~
  }
  // mClientMaxBodySize = (num * multiplier);
}

void CheckIndex(std::vector<std::string> &value) {
  if (value.size() < 1) {
    std::cerr
        << "Error: No arguments provided. At least one argument is required."
        << std::endl;
      // return or ~
  }
}

void CheckAutoIndex(std::vector<std::string> &value) {

  if (value.size() != 1) {
    std::cerr << "The number of values is not exactly 1!" << std::endl;
      // return or ~
  }
  std::string upperName = value[0];
  std::transform(upperName.begin(), upperName.end(), upperName.begin(),
                 ::toupper);
  if (value[0] != "ON" && value[0] != "OFF") {
    std::cerr << "The value is not ON or OFF!" << std::endl;
      // return or ~
  }
}

void CheckListen();
void CheckServerName();


void CheckReturn(std::vector<std::string> &value)
{
	if (value.size() != 2)
	{
    	std::cerr << "Error: Incorrect number of arguments. Exactly two arguments are required." << std::endl;
     	 // return or ~
	}
    std::string firstValue = value[0];

    for (int i = 0; i < firstValue.size(); i++)
    {
        if (isdigit(firstValue[i]) == false)
        {
        	std::cerr << "Error: Invalid character found in status code argument." << std::endl;
      		// return or ~
        }
    }

	int num = atoi(firstValue.c_str());
	if (num < 0 || num > 999)
	{
		std::cerr << "Error: The first value must be a number within the range 0-999." << std::endl;
	    // return or ~
	}
}


void CheckAlias();
void CheckLimitExcept();