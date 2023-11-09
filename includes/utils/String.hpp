#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <string>
#include <iostream>

template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

#endif