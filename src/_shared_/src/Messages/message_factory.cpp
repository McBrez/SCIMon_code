// Standard includes
#include <list>
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

// 3rd party includes
#include <easylogging++.h>
#include <flatbuffers/flatbuffers.h>

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
  Serialization::DeviceMessageT intermediateObject;

  intermediateObject.sourceId = msg->getSource().id();
  intermediateObject.desinationId = msg->getDestination().id();
  intermediateObject.msgId = msg->getMessageId();

  // Cast down the message and encode the more specific parts.
  MessageType messageType;
  // Write device message
  auto writeMsg = dynamic_pointer_cast<WriteDeviceMessage>(msg);
  if (writeMsg) {
    messageType = WRITE_DEVICE_MESSAGE;
    Serialization::WriteDeviceMessageContentT writeDeviceMessageContent;
    writeDeviceMessageContent.writeDeviceTopic =
        static_cast<Messages::Serialization::WriteDeviceTopic>(
            writeMsg->getTopic());
    intermediateObject.content.Set(writeDeviceMessageContent);
  } else {

    // Handshake message.
    auto handshakeMsg = dynamic_pointer_cast<HandshakeMessage>(msg);
    if (handshakeMsg) {
      messageType = HANDSHAKE_MESSAGE;

      Serialization::HandshakeMessageContentT handshakeMessageContent;
      for (auto statusPayload : handshakeMsg->getPayload()) {
        Serialization::statusPayloadT intermediatePayload;
        intermediatePayload.deviceId = statusPayload->getDeviceId().id();
        intermediatePayload.deviceStatus =
            static_cast<Serialization::DeviceStatus>(
                statusPayload->getDeviceStatus());
        intermediatePayload.deviceType = static_cast<Serialization::DeviceType>(
            statusPayload->getDeviceType());
        intermediatePayload.deviceName = statusPayload->getDeviceName();
        for (auto proxyId : statusPayload->getProxyIds()) {
          intermediatePayload.proxyIds.push_back(proxyId.id());
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
  }

  // Serialize the intermediate object.
  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::DeviceMessage::Pack(builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  // Wrap the buffer with two length bytes an the message type.
  int msgLen = builder.GetSize() && 0xFFFF;
  vector<unsigned char> bufferVect =
      vector<unsigned char>((const unsigned char *)buffer,
                            (const unsigned char *)buffer + builder.GetSize());

  bufferVect.insert(bufferVect.begin(), (msgLen && 0xFF00) >> 8);
  bufferVect.insert(bufferVect.begin(), (msgLen && 0x00FF));
  bufferVect.insert(bufferVect.begin(), messageType);
  bufferVect.push_back(messageType);

  return bufferVect;
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

  // Decode the payload according to the message type.
  if (MessageType::HANDSHAKE_MESSAGE == messageType) {
    const Serialization::DeviceMessage *handshakeMsg =
        Serialization::GetDeviceMessage(buffer.data());
    return this->translateHandshakeMessageContent(handshakeMsg);

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

shared_ptr<DeviceMessage>
MessageFactory::translateHandshakeMessageContent(const Serialization::DeviceMessage *msg) {
  const Serialization::HandshakeMessageContent *content =
      msg->content_as_HandshakeMessageContent();

  list<shared_ptr<StatusPayload>> statusPayloads;
  for (auto payload : *content->statusPayloads()) {
    list<UserId> proxyIds;
    for (auto proxyId : *(payload->proxyIds())) {
      proxyIds.emplace_back(UserId(static_cast<size_t>(proxyId)));
    }

    statusPayloads.emplace_back(shared_ptr<StatusPayload>(new StatusPayload(
        UserId(static_cast<size_t>(payload->deviceId())),
        static_cast<DeviceStatus>(payload->deviceStatus()), proxyIds,
        static_cast<DeviceType>(payload->deviceType()),
        payload->deviceName()->str())));
  }

  return shared_ptr<HandshakeMessage>(new HandshakeMessage(
      UserId(static_cast<size_t>(msg->sourceId())), 
      UserId(static_cast<size_t>(msg->desinationId())), statusPayloads));
}

} // namespace Messages