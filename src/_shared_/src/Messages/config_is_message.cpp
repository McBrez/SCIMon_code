#include <config_is_message.hpp>

namespace Messages {

ConfigIsMessage::ConfigIsMessage(double frequencyFrom, double frequencyTo,
                                 int measurementPoints, IsScale scale,
                                 double precision, double amplitude,
                                 bool singleFrequency)
    : frequencyFrom(frequencyFrom), frequencyTo(frequencyTo),
      measurementPoints(measurementPoints), scale(scale), precision(precision),
      amplitude(amplitude), singleFrequency(singleFrequency) {}

string ConfigIsMessage::serialize() {
  string retStr;
  retStr += "Starting frequency: ";
  retStr += this->frequencyFrom;
  return retStr;
}
} // namespace Messages