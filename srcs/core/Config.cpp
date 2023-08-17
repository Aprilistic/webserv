#include "Config.hpp"

int Config::SetConfig(const std::string &path) {
  openConfFile(path);
  parseConfFile();
}

void Config::openConfFile(const std::string &path) {
  std::ifstream confFile(path);
  if (confFile.is_open() == false) {
    std::cerr << "Error: Could not open confFile " << path << std::endl;
    exit(EXIT_FAILURE);
  }

  mConfBuffer << confFile.rdbuf();
  confFile.close();
  if (mConfBuffer.good() == false) {
    std::cerr << "Error: confFile stream " << path << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Config::tokenizer(void) {
  char ch;

  bool bDoubleQuote = false;
  bool bSingleQuote = false;
  bool bComment = false;
  bool bLiteral = false;

  std::string token;

  "server { max_body 1}";
  while (mConfBuffer.good() == true) {
    mConfBuffer >> ch;
    if (ch == '{')

      token += ch;
  }
}

// Config::parseExample(std::string filename) {
//   std::ifstream config(filename);
//   std::string line;
//   std::string currentSection;

//   if (config.is_open() == false) {
//     std::cerr << "Error: Could not open config file " << filename <<
//     std::endl; exit(EXIT_FAILURE);
//   }
//   while (std::getline(config, line)) {
//     line = line.substr(0, line.find("#")); // Remove comments
//     std::string trimmedLine;
//     std::remove_copy(line.begin(), line.end(),
//     std::back_inserter(trimmedLine),
//                      ' ');

//     if (trimmedLine.empty())
//       continue;

//     if (currentSection.empty() && trimmedLine.back() == '{') {
//       currentSection = trimmedLine.substr(0, trimmedLine.size() - 1);
//       sections[currentSection] = {};
//     } else if (!currentSection.empty() && trimmedLine == "}") {
//       currentSection.clear();
//       directives.clear();
//     } else {
//       parseLine(trimmedLine);
//       if (!currentSection.empty()) {
//         sections[currentSection].push_back(directives);
//         directives.clear();
//       }
//     }
//   }
// }

// Config::parseConfFile(void) {

//   std::string token;

//   while (mConfBuffer.empty() == false) {
//     token << mConfBuffer;

//     if (token == "{") {
//     }
//     if (token == "server") {
//       ServerBlock serverBlock;
//       serverBlock.parseServerBlock();
//       mServers.push_back(serverBlock);
//     } else {
//       std::cerr << "Error: Invalid token " << token << std::endl;
//       exit(EXIT_FAILURE);
//     }
//   }
// }