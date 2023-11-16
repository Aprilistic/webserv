#include "WebServer.hpp"

extern char **environ;

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cout << "default config" << std::endl;
  } else if (argc == 2) {

    WebServer WebServer(argv[1]);
    WebServer.Run();
  } else {
    std::cout << "Usage: ./" << argv[0] << "[config_file]" << std::endl;
  }
  return (0);
}