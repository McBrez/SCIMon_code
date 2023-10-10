#ifndef SPECTROPLOT_DATA_HPP
#define SPECTROPLOT_DATA_HPP

// 3rd party includes - Qt
#include <QMap>
// 3rd party includes - Qwt
#include <QwtInterval>
#include <QwtRasterData>

// Project includes
#include <common.hpp>

using namespace Core;

namespace Gui {

/**
 * @brief Holds the spectrogram data.
 */
class SpectrogramData : public QwtRasterData {

public:
  SpectrogramData(std::vector<double> frequencies);

  virtual QwtInterval interval(Qt::Axis axis) const override;

  virtual double value(double x, double y) const override;

  bool pushSpectrum(TimePoint timestamp, const ImpedanceSpectrum &spectrum);

private:
  /// The value ranges of the held data.
  QwtInterval intervals[3];

  /// The container that holds the data. Maps from timestamp to a vector
  /// containing impedances.
  QMap<double, std::vector<Impedance>> dataMap;

  /// Holds the frequencies that shall be displayed.
  std::vector<double> frequencies;

  /// Caches the maximum impedance value. Used for calculation of the data
  /// intervals.
  Impedance impedanceMax;
};
} // namespace Gui

#endif
