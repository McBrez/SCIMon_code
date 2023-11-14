// 3rd party includes
#include <qwt_date_scale_draw.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>

// Project includes
#include <line_plot.hpp>

namespace Gui {

LinePlot::LinePlot(QWidget *parent) : QwtPlot(parent) {
  // Set up axes.
  QwtScaleWidget *bottomAxis = this->axisWidget(QwtAxis::XBottom);
  bottomAxis->setTitle("Time");

  QwtDateScaleDraw *scaleDraw = new QwtDateScaleDraw();
  scaleDraw->setDateFormat(QwtDate::IntervalType::Minute, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Millisecond, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Second, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Minute, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Hour, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Day, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Week, "hh:mm:ss");
  scaleDraw->setDateFormat(QwtDate::Month, "hh:mm:ss");

  this->setAxisScaleDraw(QwtAxis::XBottom, scaleDraw);

  this->insertLegend(new QwtLegend());

  QwtPlotGrid *grid = new QwtPlotGrid();
  grid->attach(this);

  this->setAxisAutoScale(QwtAxis::YRight, true);
}

void LinePlot::pushData(
    const QString &title,
    const std::vector<std::tuple<TimePoint, double>> &data) {

  // Try to find the curve.
  auto it = std::find_if(
      this->curves.begin(), this->curves.end(),
      [title](QwtPlotCurve *curve) { return curve->title() == title; });

  if (it == this->curves.end()) {
    // Curve does exist. Do nothing.

    return;
  }

  // Build a polygon from the given data.
  QPolygonF points;
  for (auto &point : data) {
    size_t timestamp = std::get<TimePoint>(point).time_since_epoch().count();
    double value = std::get<double>(point);
    points << QPointF(timestamp, value);
  }

  // Append the constructed polygon to the data.
  (*this->curveData[title]) << points;

  (*it)->setSamples((*this->curveData[title]));
}

bool LinePlot::createCurve(const QString &title, QPen pen) {
  // Check if a curve with the given title already exists.
  auto it = std::find_if(
      this->curves.begin(), this->curves.end(),
      [title](QwtPlotCurve *curve) { return curve->title() == title; });

  if (it != this->curves.end()) {
    // Curve already exists. Return here.
    return false;
  } else {
    // Create the curve.
    this->curveData.insert(title, new QPolygonF());
    QwtPlotCurve *newCurve = new QwtPlotCurve(title);
    newCurve->setPen(pen);
    newCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    newCurve->setSamples(*this->curveData[title]);
    newCurve->attach(this);

    this->curves.append(newCurve);

    return true;
  }
}

bool LinePlot::removeCurve(const QString &title) {
  // Check if a curve with the given title exists.
  auto it = std::find_if(
      this->curves.begin(), this->curves.end(),
      [title](QwtPlotCurve *curve) { return curve->title() == title; });

  if (it == this->curves.end()) {
    // Curve does not exist. Return here.
    return false;
  } else {
  }
}

void LinePlot::resetData() {}

QStringList LinePlot::getCurveTitle() const {
  QStringList retVal;

  for (auto &curve : this->curves) {
    retVal << curve->title().text();
  }

  return retVal;
}

} // namespace Gui
