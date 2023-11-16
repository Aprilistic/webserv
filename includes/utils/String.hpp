#ifndef STRING_HPP
#define STRING_HPP

#include <iostream>
#include <sstream>
#include <string>

template <typename T> std::string ToString(T val) {
  std::ostringstream oss;
  oss << val;
  return oss.str();
}

#endif