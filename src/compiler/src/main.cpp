#include <iostream>
#include "DlpJitEngine.hpp"

int main(int argc, char **argv)
{
	DlpJitEngine jit;

	if (argc < 2) {
		std::cout << "Not enough arguments";
		return -1;
	}

	jit.add(argv[1]);
	jit.run();
	//parser.parse(argv[1]);
	//system("pause");

	return 0;
}

