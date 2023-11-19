// Standard includes
#include <chrono>
#include <random>

// 3rd party includes - Qt
#include <QElapsedTimer>
#include <QPen>
// 3rd party includes - Qwt
#include <QwtColorMap>
#include <QwtDateScaleDraw>
#include <QwtDateScaleEngine>
#include <QwtInterval>
#include <QwtPainter>
#include <QwtPlotLayout>
#include <QwtPlotPanner>
#include <QwtPlotRenderer>
#include <QwtPlotSpectrogram>
#include <QwtPlotZoomer>
#include <QwtScaleDraw>
#include <QwtScaleWidget>
// 3rd party includes - easylogging++
#include <easylogging++.h>

// Project includes
#include <spectroplot.hpp>
#include <spectroplot_data.hpp>
#include <utilities.hpp>

namespace Gui {

class SpectroplotDateScaleEngine : public QwtDateScaleEngine {

  virtual QwtScaleDiv divideScale(double x1, double x2, int maxMajorSteps,
                                  int maxMinorSteps,
                                  double stepSize = 0.0) const override {
    // Let the QwtDateScaleEngine calculate the div ...
    QwtScaleDiv scaleDiv = QwtDateScaleEngine::divideScale(
        x1, x2, maxMajorSteps, maxMinorSteps, stepSize);

    // ... and add the reference value to it as major tick.
    QList<double> majorTicks = scaleDiv.ticks(QwtScaleDiv::TickType::MajorTick);
    majorTicks << this->reference();
    scaleDiv.setTicks(QwtScaleDiv::TickType::MajorTick, majorTicks);

    return scaleDiv;
  }
};

Spectroplot::Spectroplot(const std::vector<double> &frequencies,
                         Core::Duration duration, QWidget *parent)
    : QwtPlot(parent),
      data(new SpectrogramData(frequencies, std::chrono::seconds(30))),
      duration(duration) {

  this->m_spectrogram.reset(new QwtPlotSpectrogram());
  this->m_spectrogram->setRenderThreadCount(
      0); // use system specific thread count
  this->m_spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);

  this->m_spectrogram->setData(
      new SpectrogramData(frequencies, std::chrono::seconds(30)));
  this->m_spectrogram->attach(this);

  const QwtInterval zInterval =
      this->m_spectrogram->data()->interval(Qt::ZAxis);

  // A color bar on the right axis
  QwtScaleWidget *rightAxis = axisWidget(QwtAxis::YRight);
  rightAxis->setTitle("|Impedance|");
  rightAxis->setColorBarEnabled(true);

  this->axisWidget(QwtAxis::XBottom)->setTitle("Frequency");

  this->setAxisScale(QwtAxis::YRight, zInterval.minValue(),
                     zInterval.maxValue());
  this->setAxisVisible(QwtAxis::YRight);

  /*
  SpectroplotDateScaleEngine *scaleEngine = new SpectroplotDateScaleEngine();
  scaleEngine->setAttribute(QwtScaleEngine::IncludeReference);
  this->setAxisScaleEngine(QwtAxis::YLeft, scaleEngine);
*/

  QwtDateScaleDraw *scaleDraw = new QwtDateScaleDraw();
  scaleDraw->setDateFormat(QwtDate::IntervalType::Minute, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Millisecond, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Second, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Minute, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Hour, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Day, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Week, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Month, "hh:mm:ss");

  this->setAxisScaleDraw(QwtAxis::YLeft, scaleDraw);

  this->plotLayout()->setAlignCanvasToScales(true);

  this->m_spectrogram->setColorMap(new QwtLinearColorMap());
  this->axisWidget(QwtAxis::YRight)
      ->setColorMap(zInterval, new QwtLinearColorMap());
}

void Spectroplot::pushSpectrum(TimePoint timestamp,
                               const ImpedanceSpectrum &spectrum) {

  TimePoint removeBeforeTs = Core::getNow() - this->duration;

  // Remove old data ...
  this->data->removeBefore(removeBeforeTs);

  // ... push the new one ...
  this->data->pushSpectrum(timestamp, spectrum);
  this->m_spectrogram->setData(this->data);

  // ... and adjust the intervals.
  QwtInterval zInterval = this->data->interval(Qt::ZAxis);
  this->axisWidget(QwtAxis::YRight)
      ->setColorMap(zInterval, new QwtLinearColorMap());
  this->rescalePlot();

  this->replot();
}

void Spectroplot::rescalePlot() {
  QwtInterval xInterval = this->data->interval(Qt::XAxis);
  QwtInterval yInterval = this->data->interval(Qt::YAxis);
  QwtInterval zInterval = this->data->interval(Qt::ZAxis);

  this->setAxisScale(QwtAxis::XBottom, xInterval.minValue(),
                     xInterval.maxValue());
  this->setAxisScale(QwtAxis::YLeft, yInterval.minValue(),
                     yInterval.maxValue());
  this->setAxisScale(QwtAxis::YRight, zInterval.minValue(),
                     zInterval.maxValue());

  QList<double> majorTicks;
  int stepCount = 5;
  double stepSize =
      (yInterval.maxValue() - yInterval.minValue()) / (double)stepCount;
  ;
  for (int i = 0; i < stepCount; i++) {
    majorTicks << yInterval.maxValue() - i * stepSize;
  }
  majorTicks << yInterval.minValue();
  QwtScaleDiv scaleDiv(yInterval.minValue(), yInterval.maxValue(),
                       QList<double>{}, QList<double>{}, majorTicks);
  this->setAxisScaleDiv(QwtAxis::YLeft, scaleDiv);
}

} // namespace Gui
