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

void splitImpedanceSpectrum(
    const std::vector<ImpedanceSpectrum> &isSpectrum,
    std::vector<std::vector<std::vector<double>>> &array) {

  if (isSpectrum.size() == 0) {
    return;
  }

  size_t frequencyCount = isSpectrum[0].size();
  if (frequencyCount == 0) {
    return;
  }

  array.reserve(isSpectrum.size());
  for (auto &spectrum : isSpectrum) {
    array.emplace_back(std::vector<std::vector<double>>());
    array.back().reserve(frequencyCount);
    for (auto &impedancePoint : spectrum) {
      array.back().emplace_back(std::vector<double>());
      double real = std::get<Impedance>(impedancePoint).real();
      double imag = std::get<Impedance>(impedancePoint).imag();
      array.back().back() = {real, imag};
    }
  }
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

void splitImpedance(const std::vector<Impedance> &impedanceVec,
                    std::vector<double> &realVec,
                    std::vector<double> &imagVec) {
  realVec.reserve(impedanceVec.size());
  imagVec.reserve(impedanceVec.size());

  for (auto impedance : impedanceVec) {
    realVec.push_back(impedance.real());
    imagVec.push_back(impedance.imag());
  }
}

void joinImpedanceSpectrum(
    const std::vector<std::vector<std::vector<double>>> &array,
    const std::vector<double> &spectrumMapping,
    std::vector<ImpedanceSpectrum> &impedanceSpectrums) {

  impedanceSpectrums.reserve(array.size());
  size_t frequencyCount = spectrumMapping.size();

  for (size_t timeIdx = 0; timeIdx < array.size(); timeIdx++) {
    impedanceSpectrums.emplace_back(ImpedanceSpectrum());
    for (size_t frequencyIdx = 0; frequencyIdx < frequencyCount;
         frequencyIdx++) {

      double real = array[timeIdx][frequencyIdx][0];
      double imag = array[timeIdx][frequencyIdx][1];
      impedanceSpectrums[timeIdx].emplace_back(
          ImpedancePoint(spectrumMapping[frequencyIdx], Impedance(real, imag)));
    }
  }
}

void joinImpedanceSpectrum(const std::vector<double> &frequencies,
                           const std::vector<Impedance> &impedances,
                           ImpedanceSpectrum &impedanceSpectrum) {

  for (int i = 0; i < frequencies.size(); i++) {
    impedanceSpectrum.emplace_back(
        ImpedancePoint(frequencies[i], impedances[i]));
  }
}

void splitImpedanceSpectrum(const std::vector<ImpedanceSpectrum> &isSpectrum,
                            std::vector<std::vector<double>> &frequencies,
                            std::vector<std::vector<double>> &realVec,
                            std::vector<std::vector<double>> &imagVec) {

  if (isSpectrum.size() == 0) {
    return;
  }

  frequencies.reserve(isSpectrum.size());
  realVec.reserve(isSpectrum.size());
  imagVec.reserve(isSpectrum.size());

  for (auto &spectrum : isSpectrum) {
    frequencies.emplace_back(std::vector<double>());
    realVec.emplace_back(std::vector<double>());
    imagVec.emplace_back(std::vector<double>());

    frequencies.back().reserve(spectrum.size());
    realVec.back().reserve(spectrum.size());
    imagVec.back().reserve(spectrum.size());

    for (auto &impedancePoint : spectrum) {
      frequencies.back().push_back(std::get<0>(impedancePoint));
      realVec.back().push_back(std::get<1>(impedancePoint).real());
      realVec.back().push_back(std::get<1>(impedancePoint).imag());
    }
  }
}

void splitImpedanceSpectrum(const std::vector<ImpedanceSpectrum> &isSpectrum,
                            std::vector<std::vector<double>> &realVec,
                            std::vector<std::vector<double>> &imagVec) {

  if (isSpectrum.size() == 0) {
    return;
  }

  realVec.reserve(isSpectrum.size());
  imagVec.reserve(isSpectrum.size());

  for (auto &spectrum : isSpectrum) {
    realVec.emplace_back(std::vector<double>());
    imagVec.emplace_back(std::vector<double>());

    realVec.back().reserve(spectrum.size());
    imagVec.back().reserve(spectrum.size());

    for (auto &impedancePoint : spectrum) {
      realVec.back().push_back(std::get<1>(impedancePoint).real());
      realVec.back().push_back(std::get<1>(impedancePoint).imag());
    }
  }
}

} // namespace Utilities
