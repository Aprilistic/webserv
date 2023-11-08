#ifndef HASH_HPP
#define HASH_HPP

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

unsigned long simpleHash(const std::string &str);
std::string hasToString(unsigned long hash);
std::string generateUniqueHash(const std::string &uriPath);

#endif