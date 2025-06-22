#pragma once

#include <iostream>
#include <string>

inline std::string input(const std::string& promt) {
	std::cout << promt;
	std::string buffer;
	do {
		std::getline(std::cin, buffer);
	} while (buffer.empty());

	return buffer;
}
