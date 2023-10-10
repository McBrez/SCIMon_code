// Standard includes
#include <chrono>
#include <random>
// 3rd party includes
#include <QElapsedTimer>
#include <QPen>
#include <QwtColorMap>
#include <QwtInterval>
#include <QwtPainter>
#include <QwtPlotLayout>
#include <QwtPlotPanner>
#include <QwtPlotRenderer>
#include <QwtPlotSpectrogram>
#include <QwtPlotZoomer>
#include <QwtScaleDraw>
#include <QwtScaleWidget>
#include <easylogging++.h>

// Project includes
#include <spectroplot.hpp>
#include <spectroplot_data.hpp>
#include <utilities.hpp>

namespace Gui {

Spectroplot::Spectroplot(const std::vector<double> &frequencies,
                         QWidget *parent)
    : QwtPlot(parent), data(new SpectrogramData(frequencies)) {

  m_spectrogram.reset(new QwtPlotSpectrogram());
  m_spectrogram->setRenderThreadCount(0); // use system specific thread count
  m_spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);

  m_spectrogram->setData(new SpectrogramData(frequencies));
  m_spectrogram->attach(this);

  const QwtInterval zInterval = m_spectrogram->data()->interval(Qt::ZAxis);

  // A color bar on the right axis
  QwtScaleWidget *rightAxis = axisWidget(QwtAxis::YRight);
  rightAxis->setTitle("|Impedance|");
  rightAxis->setColorBarEnabled(true);

  this->axisWidget(QwtAxis::XBottom)->setTitle("Frequency");

  setAxisScale(QwtAxis::YRight, zInterval.minValue(), zInterval.maxValue());
  setAxisVisible(QwtAxis::YRight);

  plotLayout()->setAlignCanvasToScales(true);

  this->m_spectrogram->setColorMap(new QwtLinearColorMap());
  this->axisWidget(QwtAxis::YRight)
      ->setColorMap(zInterval, new QwtLinearColorMap());
}

void Spectroplot::pushSpectrum(TimePoint timestamp,
                               const ImpedanceSpectrum &spectrum) {

  this->data->pushSpectrum(timestamp, spectrum);

  QwtInterval zInterval = this->data->interval(Qt::ZAxis);
  this->setAxisScale(QwtAxis::YRight, zInterval.minValue(),
                     zInterval.maxValue());
  this->axisWidget(QwtAxis::YRight)
      ->setColorMap(zInterval, new QwtLinearColorMap());

  this->m_spectrogram->setData(this->data);

  this->replot();
}

} // namespace Gui
