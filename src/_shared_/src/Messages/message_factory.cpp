// Standard includes
#include <sstream>

// Project includes
#include <config_device_message.hpp>
#include <handshake_message.hpp>
#include <init_device_message.hpp>
#include <is_payload.hpp>
#include <message_factory.hpp>
#include <read_device_message.hpp>
#include <read_payload_ob1.hpp>
#include <write_device_message.hpp>

namespace Messages {

MessageFactory::MessageFactory() {}

shared_ptr<DeviceMessage>
MessageFactory::decodeMessage(vector<unsigned char> &buffer) {

  list<unsigned char> bufferList(buffer.begin(), buffer.end());
  vector<unsigned char> frame = this->extractFrame(bufferList);
  if (frame.empty()) {
    return shared_ptr<DeviceMessage>();
  }

  MessageType msgTypeHint;
  return this->decodeFrame(frame, msgTypeHint);
}

vector<unsigned char>
MessageFactory::encodeMessage(shared_ptr<DeviceMessage> msg) {
  stringstream s;

  // Construct the header of the message.
  size_t sourceId = msg->getSource().id();
  s.write(reinterpret_cast<const char *>(&sourceId), sizeof(const size_t));
  size_t destinationId = msg->getDestination().id();
  s.write(reinterpret_cast<const char *>(&destinationId), sizeof(const size_t));
  int msgId = msg->getMessageId();
  s.write(reinterpret_cast<const char *>(&msgId), sizeof(int));

  // Cast down the message and encode the more specific parts.
  MessageType messageType;

  // Write device message
  auto writeMsg = dynamic_pointer_cast<WriteDeviceMessage>(msg);
  if (writeMsg) {
    messageType = WRITE_DEVICE_MESSAGE;
    WriteDeviceTopic topic = writeMsg->getTopic();
    s.write(reinterpret_cast<const char *>(&topic),
            sizeof(const WriteDeviceTopic));
  } else {

    // Handshake message.
    auto handshakeMsg = dynamic_pointer_cast<HandshakeMessage>(msg);
    if (handshakeMsg) {
      messageType = HANDSHAKE_MESSAGE;

      size_t payloadCount = handshakeMsg->getPayload().size();
      s.write(reinterpret_cast<const char *>(&payloadCount), sizeof(size_t));
      for (auto statusPayload : handshakeMsg->getPayload()) {
        size_t id = statusPayload->getDeviceId().id();
        s.write(reinterpret_cast<const char *>(&id), sizeof(size_t));

        DeviceStatus deviceStatus = statusPayload->getDeviceStatus();
        s.write(reinterpret_cast<const char *>(&deviceStatus), 1);

        DeviceType deviceType = statusPayload->getDeviceType();
        s.write(reinterpret_cast<const char *>(&deviceType), 1);

        short deviceNameLen = statusPayload->getDeviceName().length();
        s.write(reinterpret_cast<const char *>(&deviceNameLen), 2);
        s << statusPayload->getDeviceName();

        short proxyIdsLen = statusPayload->getProxyIds().size();
        s.write(reinterpret_cast<const char *>(&proxyIdsLen), 2);
        for (auto proxyId : statusPayload->getProxyIds()) {
          size_t id = proxyId.id();
          s.write(reinterpret_cast<const char *>(&id), sizeof(size_t));
        }
      }
    }

    // Read device message
    auto readMsg = dynamic_pointer_cast<ReadDeviceMessage>(msg);
    if (readMsg) {

    }

    else {
      // Init device message
      auto initMsg = dynamic_pointer_cast<InitDeviceMessage>(msg);
      if (initMsg) {
      }

      else {
        // Config device message
        auto configMsg = dynamic_pointer_cast<ConfigDeviceMessage>(msg);
        if (configMsg) {
        }
      }
    }

    // Wrap the message with the message type and the length of the payload.
    string *str = new string();
    *str = s.str();
    short msgLen = str->length() & 0xFFFF;
    const char *msgLenPtr = reinterpret_cast<const char *>(&msgLen);

    // Prepend the two length bytes in little-endian fashion.
    *str = *(msgLenPtr + 1) + *str;
    *str = *msgLenPtr + *str;

    const char *messageTypePtr = reinterpret_cast<const char *>(&messageType);

    *str = *messageTypePtr + *str + *messageTypePtr;

    return vector<unsigned char>((const unsigned char *)str->c_str(),
                                 (const unsigned char *)str->c_str() +
                                     str->length());
  }
}

bool MessageFactory::isMessageTypeTag(char byte) const {
  return MessageType::WRITE_DEVICE_MESSAGE == byte ||
         MessageType::READ_DEVICE_MESSAGE == byte ||
         MessageType::HANDSHAKE_MESSAGE == byte ||
         MessageType::INIT_DEVICE_MESSAGE == byte ||
         MessageType::CONFIG_DEVICE_MESSAGE == byte;
}

vector<unsigned char>
MessageFactory::extractFrame(list<unsigned char> &buffer) {
  while (true) {
    // Search for the next message type tag.
    unsigned char msgTypeTag = 0x00;
    bool msgTypeTagFound = false;
    while (buffer.size() > 0 && !msgTypeTagFound) {
      if (this->isMessageTypeTag((buffer.front()))) {
        msgTypeTag = buffer.front();
        msgTypeTagFound = true;
      } else {
        buffer.pop_front();
      }
    }
    if (!msgTypeTagFound) {
      return vector<unsigned char>();
    }

    // Opening message type tag detected. The next two bytes should contain
    // the length of the frame. Look ahead and find the closing message type
    // tag.
    auto bufferIt = buffer.begin();
    unsigned short len = *(++bufferIt) + (*(++bufferIt) << 8);
    if (len > buffer.size() - 1) {
      // Buffer ends prematurely. It might be the case that the closing
      // command tag has not been received yet. Return here and wait until
      // more bytes have been received.
      return vector<unsigned char>();
    }
    std::advance(bufferIt, len + 1);
    if (*bufferIt != msgTypeTag) {
      // Closing message typte tag not found. The opening message type tag did
      // not start a frame. Remove it from the buffer and start interpretation
      // again.
      buffer.pop_front();
    } else {
      // Found closing command tag. Remove the frame from the buffer and
      // return.
      std::vector<unsigned char> extractedFrame(buffer.begin(), ++bufferIt);
      buffer.erase(buffer.begin(), bufferIt);
      return extractedFrame;
    }
  }
}

shared_ptr<DeviceMessage>
MessageFactory::decodeFrame(vector<unsigned char> &buffer,
                            MessageType &msgTypeHint) {
  auto bufferIt = buffer.begin();

  // Get the message type from the first byte.
  MessageType messageType = static_cast<MessageType>(*bufferIt);

  // Get the frame length from the next two bytes.
  short msgLen = *(++bufferIt) + (*(++bufferIt) << 8);

  // Get the user id of source and destination.
  size_t sourceId = *(++bufferIt) + (*(++bufferIt) << 8) +
                    (*(++bufferIt) << 16) + (*(++bufferIt) << 24) +
                    (*(++bufferIt) << 32) + (*(++bufferIt) << 40) +
                    (*(++bufferIt) << 48) + (*(++bufferIt) << 56);
  UserId source(sourceId);
  size_t destinationId = *(++bufferIt) + (*(++bufferIt) << 8) +
                         (*(++bufferIt) << 16) + (*(++bufferIt) << 24) +
                         (*(++bufferIt) << 32) + (*(++bufferIt) << 40) +
                         (*(++bufferIt) << 48) + (*(++bufferIt) << 56);
  UserId destination(destinationId);

  // Get the message id.
  int msgId = *(++bufferIt) + (*(++bufferIt) << 8) + (*(++bufferIt) << 16) +
              (*(++bufferIt) << 24);

  // Decode the payload according to the message type.
  if (MessageType::HANDSHAKE_MESSAGE == messageType) {
    // Get the count of ids this message holds.
    size_t countIds = *(++bufferIt) + (*(++bufferIt) << 8) +
                      (*(++bufferIt) << 16) + (*(++bufferIt) << 24) +
                      (*(++bufferIt) << 32) + (*(++bufferIt) << 40) +
                      (*(++bufferIt) << 48) + (*(++bufferIt) << 56);
    // Decode the status payloads into a list.
    list<shared_ptr<StatusPayload>> statusPayloads;
    for (int i = 0; i < countIds; i++) {
      size_t objectId = *(++bufferIt) + (*(++bufferIt) << 8) +
                        (*(++bufferIt) << 16) + (*(++bufferIt) << 24) +
                        (*(++bufferIt) << 32) + (*(++bufferIt) << 40) +
                        (*(++bufferIt) << 48) + (*(++bufferIt) << 56);
      ;
      UserId id(objectId);
      DeviceStatus deviceStatus = static_cast<DeviceStatus>(*(++bufferIt));
      DeviceType deviceType = static_cast<DeviceType>(*(++bufferIt));

      short deviceTypeLen = *(++bufferIt) + (*(++bufferIt) << 8);
      string deviceName(bufferIt, bufferIt + deviceTypeLen);
      short proxyIdsLen = *(++bufferIt) + (*(++bufferIt) << 8);
      list<UserId> proxyIds;
      for (int i = 0; i < proxyIdsLen; i++) {
        size_t proxyIdValue = *(++bufferIt) + (*(++bufferIt) << 8) +
                              (*(++bufferIt) << 16) + (*(++bufferIt) << 24) +
                              (*(++bufferIt) << 32) + (*(++bufferIt) << 40) +
                              (*(++bufferIt) << 48) + (*(++bufferIt) << 56);
        proxyIds.emplace_back(UserId(proxyIdValue));
      }

      statusPayloads.emplace_back(shared_ptr<StatusPayload>(new StatusPayload(
          id, deviceStatus, proxyIds, deviceType, deviceName)));
    }

    return shared_ptr<DeviceMessage>(
        new HandshakeMessage(source, destination, statusPayloads));

  } else {
    return shared_ptr<DeviceMessage>();
  }
}

vector<unsigned char>
MessageFactory::encodeReadPayload(shared_ptr<ReadPayload> readPayload) {

  stringstream s;
  PayloadType payloadType;

  // IS payload
  auto isPayload = dynamic_pointer_cast<IsPayload>(readPayload);
  if (isPayload) {
    payloadType = PayloadType::IS_PAYLOAD;
    s.write(reinterpret_cast<const char *>(&payloadType), sizeof(PayloadType));

    unsigned int channelNumber = isPayload->getChannelNumber();
    s.write(reinterpret_cast<const char *>(&channelNumber),
            sizeof(unsigned int));

    double timestamp;

    ImpedanceSpectrum impedanceSpectrum;
  } else {

    // Status payload
    auto statusPayload = dynamic_pointer_cast<StatusPayload>(readPayload);
    if (statusPayload) {

    } else {
      // Read OB1 payload
      auto readOb1Payload = dynamic_pointer_cast<ReadPayloadOb1>(readPayload);
    }
  }

  string *str = new string();
  *str = s.str();
  return vector<unsigned char>(str->c_str(), str->c_str() + str->length());
}

shared_ptr<ReadPayload> decodeReadPayload(const vector<unsigned char> &buffer) {
  return shared_ptr<ReadPayload>();
}

} // namespace Messages