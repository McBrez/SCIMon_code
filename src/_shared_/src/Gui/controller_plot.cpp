// 3rd party includes
#include <QRandomGenerator>

// Project includes
#include <controller_plot.hpp>

namespace Gui {

ControllerPlot::ControllerPlot(QWidget *parent) : LinePlot(parent) {}

bool ControllerPlot::createCurvePair(const QString &setPointTitle,
                                     const QString &curValueTitle) {
  QColor color(QRandomGenerator::global()->generate() % 255,
               QRandomGenerator::global()->generate() % 255,
               QRandomGenerator::global()->generate() % 255);

  QPen setPointPen;
  setPointPen.setStyle(Qt::PenStyle::DashLine);
  setPointPen.setWidth(1);
  setPointPen.setColor(color);
  this->createCurve(setPointTitle, setPointPen);

  QPen curValuePen;
  curValuePen.setStyle(Qt::PenStyle::SolidLine);
  curValuePen.setWidth(2);
  curValuePen.setColor(color);
  this->createCurve(curValueTitle, curValuePen);

  return true;
}
} // namespace Gui
