// Standard includes
#include <list>
#include <sstream>

// Project includes
#include <builtin_payload_decoder.hpp>
#include <config_device_message.hpp>
#include <handshake_message.hpp>
#include <init_device_message.hpp>
#include <is_payload.hpp>
#include <message_factory.hpp>
#include <read_device_message.hpp>
#include <utilities.hpp>
#include <write_device_message.hpp>

// 3rd party includes
#include <easylogging++.h>
#include <flatbuffers/flatbuffers.h>

namespace Messages {

MessageFactory *MessageFactory::instance = nullptr;

MessageFactory::MessageFactory(list<shared_ptr<PayloadDecoder>> payloadDecoders)
    : payloadDecoders(payloadDecoders) {
  this->payloadDecoders.push_back(
      shared_ptr<PayloadDecoder>(new BuiltinPayloadDecoder()));
}

shared_ptr<DeviceMessage>
MessageFactory::decodeMessage(vector<unsigned char> &buffer) {

  list<unsigned char> bufferList(buffer.begin(), buffer.end());
  vector<unsigned char> *frame = this->extractFrame(bufferList);
  if (frame == nullptr) {
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
  // --------------------------------------------------- Write device message --
  auto writeMsg = dynamic_pointer_cast<WriteDeviceMessage>(msg);
  if (writeMsg) {
    messageType = MessageType::WRITE_DEVICE_MESSAGE;
    Serialization::WriteDeviceMessageContentT writeDeviceMessageContent;
    writeDeviceMessageContent.writeDeviceTopic =
        static_cast<Messages::Serialization::WriteDeviceTopic>(
            writeMsg->getTopic());
    intermediateObject.content.Set(writeDeviceMessageContent);
  } else {

    // ---------------------------------------------------- Handshake message --
    auto handshakeMsg = dynamic_pointer_cast<HandshakeMessage>(msg);
    if (handshakeMsg) {
      messageType = MessageType::HANDSHAKE_MESSAGE;

      Serialization::HandshakeMessageContentT handshakeMessageContent;
      handshakeMessageContent.version = this->getVersion();
      for (auto statusPayload : handshakeMsg->getPayload()) {
        Serialization::statusPayloadT *intermediatePayload =
            new Serialization::statusPayloadT();
        intermediatePayload->deviceId = statusPayload->getDeviceId().id();
        intermediatePayload->deviceStatus =
            static_cast<Serialization::DeviceStatus>(
                statusPayload->getDeviceStatus());
        intermediatePayload->deviceType =
            static_cast<Serialization::DeviceType>(
                statusPayload->getDeviceType());
        intermediatePayload->deviceName = statusPayload->getDeviceName();
        for (auto proxyId : statusPayload->getProxyIds()) {
          intermediatePayload->proxyIds.push_back(proxyId.id());
        }
        handshakeMessageContent.statusPayloads.emplace_back(
            unique_ptr<Serialization::statusPayloadT>(intermediatePayload));
      }
      intermediateObject.content.Set(handshakeMessageContent);
    }

    // -------------------------------------------------- Read device message --
    auto readMsg = dynamic_pointer_cast<ReadDeviceMessage>(msg);
    if (readMsg) {
      messageType = MessageType::READ_DEVICE_MESSAGE;
      Serialization::ReadDeviceMessageContentT readDeviceMessageContent;
      readDeviceMessageContent.readDeviceTopic =
          static_cast<Serialization::ReadDeviceTopic>(readMsg->getTopic());
      readDeviceMessageContent.readPayload = readMsg->getReadPaylod()->bytes();

      intermediateObject.content.Set(readDeviceMessageContent);
    }

    else {
      // ------------------------------------------------ Init device message --
      auto initMsg = dynamic_pointer_cast<InitDeviceMessage>(msg);
      if (initMsg) {
        Serialization::InitDeviceMessageContentT initDeviceMessageContent;
        initDeviceMessageContent.initPayoad = initMsg->returnPayload()->bytes();

        intermediateObject.content.Set(initDeviceMessageContent);
      }

      else {
        // -------------------------------------------- Config device message --
        auto configMsg = dynamic_pointer_cast<ConfigDeviceMessage>(msg);
        if (configMsg) {
        }
      }
    }
  }

  // Serialize the intermediate object.
  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(
      Serialization::DeviceMessage::Pack(builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  // Wrap the buffer with two length bytes an the message type.
  int msgLen = builder.GetSize() & 0xFFFF;
  vector<unsigned char> bufferVect =
      vector<unsigned char>((const unsigned char *)buffer,
                            (const unsigned char *)buffer + builder.GetSize());

  bufferVect.insert(bufferVect.begin(), (msgLen & 0xFF00) >> 8);
  bufferVect.insert(bufferVect.begin(), (msgLen & 0x00FF));
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

vector<unsigned char> *
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
      return nullptr;
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
      return nullptr;
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
      std::vector<unsigned char> *extractedFrame =
          new std::vector<unsigned char>(buffer.begin(), ++bufferIt);
      buffer.erase(buffer.begin(), bufferIt);
      return extractedFrame;
    }
  }
}

shared_ptr<DeviceMessage>
MessageFactory::decodeFrame(vector<unsigned char> *buffer,
                            MessageType &msgTypeHint) {
  auto bufferIt = buffer->begin();

  // Get the message type from the first byte.
  MessageType messageType = static_cast<MessageType>(*bufferIt);

  // Get the frame length from the next two bytes.
  short msgLen = *(++bufferIt) + (*(++bufferIt) << 8);

  // Create a new vector that only containt the flatbuffer payload. flatbuffer
  // will take ownership of this pointer.
  vector<unsigned char> *flatbufferPayload =
      new vector<unsigned char>(++bufferIt, buffer->end() - 1);

  // Decode the payload according to the message type.
  const Serialization::DeviceMessageT *handshakeMsg =
      Serialization::GetDeviceMessage(flatbufferPayload->data())->UnPack();
  if (MessageType::HANDSHAKE_MESSAGE == messageType) {
    return this->translateMessageContent(
        UserId(static_cast<size_t>(handshakeMsg->sourceId)),
        UserId(static_cast<size_t>(handshakeMsg->desinationId)),
        handshakeMsg->content.AsHandshakeMessageContent());
  }

  else if (MessageType::WRITE_DEVICE_MESSAGE == messageType) {
    return this->translateMessageContent(
        UserId(static_cast<size_t>(handshakeMsg->sourceId)),
        UserId(static_cast<size_t>(handshakeMsg->desinationId)),
        handshakeMsg->content.AsWriteDeviceMessageContent());
  } else {
    return shared_ptr<DeviceMessage>();
  }
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::HandshakeMessageContentT *handshakeContent) {

  list<shared_ptr<StatusPayload>> statusPayloads;
  for (int i = 0; i < handshakeContent->statusPayloads.size(); i++) {
    list<UserId> proxyIds;

    for (auto proxyId : handshakeContent->statusPayloads[i]->proxyIds) {
      proxyIds.emplace_back(UserId(static_cast<size_t>(proxyId)));
    }

    statusPayloads.emplace_back(shared_ptr<StatusPayload>(new StatusPayload(
        UserId(
            static_cast<size_t>(handshakeContent->statusPayloads[i]->deviceId)),
        static_cast<DeviceStatus>(
            handshakeContent->statusPayloads[i]->deviceStatus),
        proxyIds,
        static_cast<DeviceType>(
            handshakeContent->statusPayloads[i]->deviceType),
        handshakeContent->statusPayloads[i]->deviceName)));
  }

  return shared_ptr<HandshakeMessage>(new HandshakeMessage(
      sourceId, destinationId, statusPayloads, handshakeContent->version));
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::WriteDeviceMessageContentT *writeDeviceContent) {

  return shared_ptr<WriteDeviceMessage>(new WriteDeviceMessage(
      sourceId, destinationId,
      static_cast<WriteDeviceTopic>(writeDeviceContent->writeDeviceTopic)));
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::ReadDeviceMessageContentT *readDeviceContent) {

  // Try to parse the payload.
  ReadPayload *decodedPayload = nullptr;
  for (auto payloadDecoder : this->payloadDecoders) {
    decodedPayload =
        payloadDecoder->decodeReadPayload(readDeviceContent->readPayload);
    if (decodedPayload != nullptr) {
      break;
    }
  }

  if (!decodedPayload) {
    // Was not able to decode a read payload from the buffer.
    return shared_ptr<DeviceMessage>();
  }

  return shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(
      sourceId, destinationId,
      static_cast<ReadDeviceTopic>(readDeviceContent->readDeviceTopic),
      decodedPayload, nullptr));
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::InitDeviceMessageContentT *initDeviceContent) {

  return shared_ptr<DeviceMessage>();
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::ConfigDeviceMessageContentT *configDeviceContent) {

  return shared_ptr<DeviceMessage>();
}

MessageFactory *MessageFactory::createInstace(
    list<shared_ptr<PayloadDecoder>> payloadDecoders) {
  if (MessageFactory::instance == nullptr) {
    MessageFactory::instance = new MessageFactory(payloadDecoders);
    return MessageFactory::instance;
  } else {
    return nullptr;
  }
}

MessageFactory *MessageFactory::getInstace() {
  return MessageFactory::instance;
}

string MessageFactory::getVersion() const {
#ifdef SCIMON_MESSAGE_LIB_VERSION
  return SCIMON_MESSAGE_LIB_VERSION;
#else
  return "UNDEFINED";
#endif
}

} // namespace Messages