#include "Core.hpp"

void sendError(const std::string &msg) {
  std::cerr << msg << std::endl;
  exit(1);
}