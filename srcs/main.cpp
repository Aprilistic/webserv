#include "Core.hpp"

int main(int ac, char **av) {
  if (ac == 2) {
    Server();
  } else {
    std::cout << "Usage: ./webserv [conf_file]" << std::endl;
  }
  return (0);
}