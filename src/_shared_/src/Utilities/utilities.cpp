#include <utilities.hpp>

namespace Utilities {

std::vector<std::string> split(const std::string &str, unsigned char token) {
  std::vector<std::string> retVal;

  auto tokenStart = str.begin();
  auto tokenEnd = str.end();
  std::string::const_iterator it;

  std::vector<std::string::const_iterator> tokenPositions;
  std::string::const_iterator tokenPosition = str.cbegin();
  do {
    tokenPosition = std::find(tokenPosition, str.cend(), token);
    tokenPositions.push_back(tokenPosition);
    if (tokenPosition != str.cend())
      tokenPosition++;
  } while (tokenPosition != str.cend());

  std::string::const_iterator strStart = str.cbegin();
  for (auto strEnd : tokenPositions) {
    std::string splitStr = "";
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
                            std::vector<double> &frequencies,
                            std::vector<Impedance> &impedance) {

  frequencies.reserve(isSpectrum.size());
  for (auto impedancePoint : isSpectrum) {
    frequencies.push_back(get<0>(impedancePoint));
    impedance.push_back(get<1>(impedancePoint));
  }
}

} // namespace Utilities