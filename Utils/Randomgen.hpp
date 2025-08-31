#pragma once

#include <cstdint>
#include <random>
#include <string>

std::string gen_access_token(std::uint32_t len = 32) {
	const std::string chars_set =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::random_device rnd_dev;
	std::mt19937 gen(rnd_dev());

	std::string result;
	result.reserve(len);

	std::uniform_int_distribution<std::uint32_t> dist(0, chars_set.length() - 1);

	for (std::uint32_t i = 0; i < len; ++i) {
		result += chars_set.at(dist(gen));
	}

	return result;
}