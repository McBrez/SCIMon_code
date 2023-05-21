#include <utilities.hpp>

namespace Utilities {

vector<string> split(const string &str, unsigned char token) {
  vector<string> retVal;

  auto tokenStart = str.begin();
  auto tokenEnd = str.end();
  string::const_iterator it;

  vector<string::const_iterator> tokenPositions;
  string::const_iterator tokenPosition = str.cbegin();
  do {
    tokenPosition = find(tokenPosition, str.cend(), token);
    tokenPositions.push_back(tokenPosition);
    if (tokenPosition != str.cend())
      tokenPosition++;
  } while (tokenPosition != str.cend());

  string::const_iterator strStart = str.cbegin();
  for (auto strEnd : tokenPositions) {
    string splitStr = "";
    for (auto ch = strStart; ch != strEnd; ++ch) {
      splitStr += *ch;
    }
    retVal.push_back(splitStr);
    if (strEnd != str.cend()) {
      strStart = ++strEnd;
    }
  }

  return retVal;
}

void splitImpedanceSpectrum(const ImpedanceSpectrum &isSpectrum,
                            vector<double> &frequencies,
                            vector<Impedance> &impedance) {

  frequencies.reserve(isSpectrum.size());
  for (auto impedancePoint : isSpectrum) {
    frequencies.push_back(get<0>(impedancePoint));
    impedance.push_back(get<1>(impedancePoint));
  }
}

} // namespace Utilities