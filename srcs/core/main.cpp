#include "/Users/hyeoan/goinfre/web/includes/core/Config.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: Invalid arguments" << std::endl;
		return (1);
	}
	Config config(argv[1]);
	// config.SetConfig(argv[1]);
	return (0);
}