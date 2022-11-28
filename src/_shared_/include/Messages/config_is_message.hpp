#ifndef CONFIG_IS_MESSAGE_HPP
#define CONFIG_IS_MESSAGE_HPP

#include <config_device_message.hpp>

namespace Messages {

enum IsScale { LINEAR, LOGARITHMIC };

class ConfigIsMessage : public ConfigDeviceMessage {
public:
  ConfigIsMessage(double frequencyFrom, double frequencyTo,
                  int measurementPoints, IsScale scale, double precision,
                  double amplitude, bool singleFrequency);

  virtual string serialize() override;

  const double frequencyFrom;
  const double frequencyTo;
  const int measurementPoints;
  const IsScale scale;
  const double precision;
  const double amplitude;
  const bool singleFrequency;
};
} // namespace Messages

#endif