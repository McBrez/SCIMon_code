#ifndef READ_DEVICE_MESSAGE_HPP
#define READ_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>
#include <read_payload.hpp>
#include <write_device_message.hpp>

using namespace Devices;

namespace Messages {

/**
 * @brief Identifies the reason of the read message.
 */
enum ReadDeviceTopic {
  /// Invalid topic
  READ_TOPIC_INVALID = 0x00,

  /// The message contains the status of the device.
  READ_TOPIC_DEVICE_STATUS = 0x01,

  /// The target device could not be reached.
  READ_TOPIC_FAILED_RESPONSE = 0x02,

  /// A device specific message.
  READ_TOPIC_DEVICE_SPECIFIC_MSG = 0x03,

  /// Response that contains previously requested data.
  READ_TOPIC_DATA_RESPONSE = 0x04,

  /// Response to a request keys message. Message contains the data keys of the
  /// object.
  READ_TOPIC_KEY_RESPONSE = 0x05
};

/**
 * An message  that has been generated by a device in response to a
 * DeviceWriteMessage.
 */
class ReadDeviceMessage : public DeviceMessage {
public:
  /**
   * @brief Constructs the read device message with the given topic and
   * payload.
   * @param source Reference to the source of this message.
   * @param destination Reference to the destination of this message.
   * @param topic The topic of the message.
   * @param readPayload The payload of the message.
   * @param originalMessage Reference to the message, this message is the
   * response to.
   */
  ReadDeviceMessage(UserId source, UserId destination, ReadDeviceTopic topic,
                    ReadPayload *readPayload,
                    std::shared_ptr<WriteDeviceMessage> originalMessage);

  /**
   * @brief Constructs the read device message with the given topic and
   * payload.
   * @param source Reference to the source of this message.
   * @param destination Reference to the destination of this message.
   * @param topic The topic of the message.
   * @param readPayload The payload of the message.
   * @param originalMessage Reference to the message, this message is the
   * response to.
   */
  ReadDeviceMessage(UserId source, UserId destination, ReadDeviceTopic topic,
                    std::shared_ptr<ReadPayload> readPayload,
                    std::shared_ptr<WriteDeviceMessage> originalMessage);

  /**
   * @brief Serializes the message into a human-readable string.
   * @return The string representation of the message.
   */
  virtual std::string serialize() override;

  /**
   * @brief Return the payload of the message.
   * @return The payload, held by this message.
   */
  std::shared_ptr<ReadPayload> getReadPaylod();

  /**
   * @brief Returns the message, this object is the response to.
   * @return Reference to the message, this object is the respons to.
   */
  std::shared_ptr<WriteDeviceMessage> getOriginalMessage();

  /**
   * @brief Returns the topic of this message.
   * @return The topic of this message.
   */
  ReadDeviceTopic getTopic();

private:
  /// Reference to the messsage this message is the response to.
  std::shared_ptr<WriteDeviceMessage> originalMessage;

  /// Reference to the payload.
  std::shared_ptr<ReadPayload> readPayload;

  /// The topic of the message.
  ReadDeviceTopic topic;
};
} // namespace Messages

#endif
