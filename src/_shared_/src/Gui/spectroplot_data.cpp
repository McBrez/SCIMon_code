// Project includes
#include <spectroplot_data.hpp>
#include <utilities.hpp>

namespace Gui {

SpectrogramData::SpectrogramData(std::vector<double> frequencies,
                                 std::chrono::seconds retentionPeriod)
    : frequencies(frequencies), impedanceMax(0.0, 0.0) {

  this->intervals[Qt::XAxis] =
      QwtInterval(0, *std::max_element(frequencies.begin(), frequencies.end()));
  this->intervals[Qt::YAxis] = QwtInterval(0.0, 0.0);
  this->intervals[Qt::ZAxis] = QwtInterval(0.0, 0.0);

  setAttribute(QwtRasterData::WithoutGaps, true);

  // Ensure that frequencies are sorted.
  std::sort(this->frequencies.begin(), this->frequencies.end());
}

QwtInterval SpectrogramData::interval(Qt::Axis axis) const {
  if (axis >= 0 && axis <= 2)
    return this->intervals[axis];

  return QwtInterval();
}

double SpectrogramData::value(double x, double y) const {
  // Get the nearest timestamp.
  auto itTimestamp =
      std::lower_bound(this->dataMap.keyBegin(), this->dataMap.keyEnd(), y);
  if (itTimestamp == this->dataMap.keyEnd()) {
    return 0.0;
  }
  double nearestTimestamp = *itTimestamp;

  // Get the nearest frequency and corresponding index.
  auto itFreq =
      std::lower_bound(this->frequencies.begin(), this->frequencies.end(), x);
  if (itFreq == this->frequencies.end()) {
    return 0.0;
  }
  size_t i = std::distance(this->frequencies.begin(), itFreq);

  return std::abs(this->dataMap[nearestTimestamp][i]);
}

bool SpectrogramData::pushSpectrum(TimePoint timestamp,
                                   const ImpedanceSpectrum &spectrum) {

  // Check if the impedance spectrum matches to the configured frequencies.
  std::vector<double> impedanceFrequencies;
  std::vector<Impedance> impedances;
  Utilities::splitImpedanceSpectrum(spectrum, impedanceFrequencies, impedances);
  if (impedanceFrequencies != this->frequencies) {
    // Frequencies do not match. Clear the data mapping and adjust the
    // frequencies.
    this->dataMap.clear();
    this->frequencies = impedanceFrequencies;

    this->intervals[Qt::XAxis] =
        QwtInterval(0, *std::max_element(this->frequencies.begin(),
                                         this->frequencies.end()));
    this->intervals[Qt::YAxis] = QwtInterval(0.0, 0.0);
    this->intervals[Qt::ZAxis] = QwtInterval(0.0, 0.0);
    this->impedanceMax = Impedance(0.0, 0.0);
  }

  // Add the spectrum to the data container.
  double timestampDouble =
      static_cast<double>(timestamp.time_since_epoch().count());
  this->dataMap[timestampDouble] = impedances;

#if 0
  // Remove timestamp/value pairs that are beyond the retention period.
  QMap<double, std::vector<Impedance>> tempMap;
  TimePoint now = Core::getNow();

  for(int i = 0; i < this->dataMap.size(); ) {

  }



  for (auto &keyValuePair : this->dataMap) {

  }
#endif
  // Adjust the intervals.
  // Y axis - timestamps
  QList<double> keys = this->dataMap.keys();
  double timestampMin = *std::min_element(keys.begin(), keys.end());
  double timestampMax = *std::max_element(keys.begin(), keys.end());

  this->intervals[Qt::YAxis].setInterval(timestampMin, timestampMax);
  // Z axis - Impedance
  // Check if the new spectrum added a new maximum.
  Impedance newMaxImpedance =
      *std::max_element(impedances.begin(), impedances.end());
  if (newMaxImpedance > this->impedanceMax) {
    this->impedanceMax = newMaxImpedance;
    this->intervals[Qt::ZAxis].setInterval(0.0, std::abs(this->impedanceMax));
  }

  return true;
}

} // namespace Gui
