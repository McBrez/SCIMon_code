#ifndef UTILITIES_HPP
#define UTILITIES_HPP

// Standard includes
#include <string>
#include <vector>

using namespace std;

namespace Utilities {

/**
 * @brief Splits a string at the given token and returns a list of strings.
 *
 * @param str The string that shall be split.
 * @param token The token at which shall be split.
 * @return The splitted strings.
 */
vector<string> split(const string &str, unsigned char token);
} // namespace Utilities
#endif