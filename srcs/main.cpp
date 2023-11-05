#include "WebServer.hpp"
#include "Common.hpp"
#include "ConfigMap.hpp"

extern char **environ;

void ExitHandler(void) { system("leaks webserv"); }

int main(int argc, char **argv) {
  // atexit(ExitHandler);
  if (argc == 1) {
    std::cout << "default config" << std::endl;
  } else if (argc == 2) {

  WebServer WebServer(argv[1]);

  Node *location = Common::mConfigMap->GetConfigNode(
      80, "localhost", "/restricted_area.bla");
  if (location == NULL) {
    std::cout << "NULL" << std::endl;
  } else {
    for (std::map<std::string, std::vector<std::string> >::iterator it =
            location->mDirectives.begin();
        it != location->mDirectives.end(); ++it) {
      std::cout << it->first << " ";
      for (size_t i = 0; i < it->second.size(); i++) {
        std::cout << it->second[i] << std::endl;
      }
    }
  }

  // WebServer.Run();
  } else {
    std::cout << "Usage: ./" << argv[0] << "[config_file]" << std::endl;
  }
  return (0);
}