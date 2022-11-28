#ifndef UTILITY_HPP
#define UTILITY_HPP

// Standard includes
#include <string>
#include <vector>

using namespace std;

namespace Utility {

/**
 * @brief Splits a string at the given token and returns a list of strings.
 *
 * @param str The string that shall be split.
 * @param token The token at which shall be split.
 * @return The splitted strings.
 */
vector<string> split(const string &str, char token);
} // namespace Utility
#endif