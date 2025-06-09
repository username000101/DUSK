#include <charconv>
#include <string>

bool is_number(const std::string& str) {
    if (str.empty()) return false;

    long long val;
    auto start = str.data();
    auto end = str.data() + str.size();

    auto result = std::from_chars(start, end, val);
    return result.ec == std::errc() && result.ptr == end;
}