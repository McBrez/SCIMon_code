#include <utility.hpp>

namespace Utility {

vector<string> split(const string &str, char token) {
  vector<string> retVal;

  auto tokenStart = str.begin();
  auto tokenEnd = str.end();
  string::const_iterator it;

  std::vector<string::const_iterator> tokenPositions;
  string::const_iterator tokenPosition = str.begin();
  do {
    tokenPosition = std::find(tokenPosition, str.end(), token);
    tokenPositions.push_back(tokenPosition);
    if (tokenPosition != str.end())
      tokenPosition++;
  } while (tokenPosition != str.end());

  string::const_iterator strStart = str.begin();
  for (auto strEnd : tokenPositions) {
    string splitStr = "";
    for (auto ch = strStart; ch != strEnd; ++ch) {
      splitStr += *ch;
    }
    retVal.push_back(splitStr);
    if (strEnd != str.end()) {
      strStart = ++strEnd;
    }
  }

  return retVal;
}

} // namespace Utility