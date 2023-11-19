// 3rd party includes
#include <QRandomGenerator>

// Project includes
#include <controller_plot.hpp>

namespace Gui {

ControllerPlot::ControllerPlot(QString currentPressureStr,
                               QString setPointPressureStr,
                               Core::Duration duration, QWidget *parent)
    : LinePlot(parent, duration), currentPressureStr(currentPressureStr),
      setPointPressureStr(setPointPressureStr) {}

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

bool ControllerPlot::createCurvePair(const QString &curveTitle) {

  // Do not add the curve, if it already exists.
  if (this->getCurveTitles().contains(curveTitle)) {
    return false;
  }

  if (curveTitle.contains(this->currentPressureStr)) {
    // Construct the string for the set point pressure curve from the curve
    // title.
    QString setPointPressureTitle = curveTitle;
    setPointPressureTitle.replace(this->currentPressureStr, "");
    setPointPressureTitle += this->setPointPressureStr;

    this->createCurvePair(setPointPressureTitle, curveTitle);

    return true;
  } else if (curveTitle.contains(this->setPointPressureStr)) {
    // Construct the string for the current pressure curve from the curve
    // title.
    QString currentValueTitle = curveTitle;
    currentValueTitle.replace(this->setPointPressureStr, "");
    currentValueTitle += this->currentPressureStr;

    this->createCurvePair(curveTitle, currentValueTitle);

    return true;
  } else {
    return false;
  }
}

} // namespace Gui
