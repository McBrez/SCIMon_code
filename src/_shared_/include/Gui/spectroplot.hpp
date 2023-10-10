#ifndef SPECTROPLOT_HPP
#define SPECTROPLOT_HPP

// 3rd party include
#include <QTimer>
#include <QwtPlot>
#include <qwt_plot_spectrogram.h>

// Project includes
#include <common.hpp>
#include <spectroplot_data.hpp>

using namespace Core;

namespace Gui {
class Spectroplot : public QwtPlot {
  Q_OBJECT

public:
  Spectroplot(const std::vector<double> &frequencies,
              QWidget *parent = Q_NULLPTR);

signals:
  void rendered(const QString &status);

public slots:
  void pushSpectrum(TimePoint timestamp, const ImpedanceSpectrum &spectrum);

private:
  std::unique_ptr<QwtPlotSpectrogram> m_spectrogram;
  SpectrogramData *data;
};
} // namespace Gui
#endif
