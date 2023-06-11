#ifndef WRITE_DEVICE_MESSAGE_HPP
#define WRITE_DEVICE_MESSAGE_HPP

// Standard includes
#include <variant>

// Project includes
#include <device_message.hpp>
#include <payload.hpp>

using namespace Devices;
using AdditionalData = std::variant<bool, int, float, std::string>;

namespace Messages {

/**
 * @brief Identifies the reasons for the write operation.
 */
enum WriteDeviceTopic {
  /// Invalid topic
  WRITE_TOPIC_INVALID = 0x00,
  /// The device shall start its operation with a previously sent configuration.
  WRITE_TOPIC_RUN = 0x01,
  /// The device shall stop its operation.
  WRITE_TOPIC_STOP = 0x02,
  /// The message contains data for the device.
  WRITE_TOPIC_DATA = 0x03,
  /// The targeted device shall respond with its current state.
  WRITE_TOPIC_QUERY_STATE = 0x04,
  /// A device specific write topic.
  WRITE_TOPIC_DEVICE_SPECIFIC = 0x05
};

/**
 * @brief A message that is meant to trigger certain processes in a device.
 */
class WriteDeviceMessage : public DeviceMessage {
public:
  /**
   * @brief Creates a message with the given topic.
   * @param source Reference to the source of this message.
   * @param destination Reference to the destination of this message.
   * @param topic The topic of the message.
   */
  WriteDeviceMessage(UserId source, UserId destination, WriteDeviceTopic topic);
  /**
   * @brief Creates a message with the given topic and appends additional data.
   * @param source Reference to the source of this message.
   * @param destination Reference to the destination of this message.
   * @param topic The topic of the message.
   * @param additionalData Additional data, that shall be appended to the
   * message.
   */
  WriteDeviceMessage(UserId source, UserId destination, WriteDeviceTopic topic,
                     AdditionalData additionalData);

  /**
   * @brief Returns the additional data, held by the message.
   * @return The additional data object, held by the message.
   */
  AdditionalData getAdditionalData();

  /**
   * @brief Returns the topic of the message.
   * @return The topic of the message.
   */
  WriteDeviceTopic getTopic();

  /**
   * @brief Serializes the contents of the message into a human-readable string.
   * @return The string representation of the message.
   */
  virtual std::string serialize() override;

  /**
   * @brief Returns the payload held by this object.
   * @return Pointer to the payload held by this object. (May be nullptr).
   */
  std::shared_ptr<Payload> getPayload();

private:
  /// The additional data.
  AdditionalData additionalData;
  /// The topic of the message.
  WriteDeviceTopic topic;
  /// May hold a payload.
  std::shared_ptr<Payload> payload;
};
} // namespace Messages

#endif
