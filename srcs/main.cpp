#include "Core.hpp"
#include "Master.hpp"

int main(int ac, char **av) {
  if (ac == 2) {
    Master master(av[1]);
  } else {
    std::cout << "Usage: ./webserv [conf_file]" << std::endl;
  }
  return (0);
