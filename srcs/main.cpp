#include "Config.hpp"

// void leaks(void)
// {
// 	system("leaks webserv");
// }

int main(int argc, char** argv)
{
	// atexit(leaks);
	if (argc != 2)
	{
		std::cerr << "Error: Invalid arguments" << std::endl;
		return (1);
	}
	Config config(argv[1]);
	return (0);
}