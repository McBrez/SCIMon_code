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

// Generated includs
#include <status_payload_generated.h>

namespace Messages {

MessageFactory *MessageFactory::instance = nullptr;

MessageFactory::MessageFactory(list<shared_ptr<PayloadDecoder>> payloadDecoders)
    : payloadDecoders(payloadDecoders) {
  this->payloadDecoders.push_back(
      shared_ptr<PayloadDecoder>(new BuiltinPayloadDecoder()));
}

shared_ptr<DeviceMessage>
MessageFactory::decodeMessage(vector<unsigned char> &buffer) {

  // Transform the vector to a list.
  list<unsigned char> bufferList(buffer.begin(), buffer.end());

  // Try to extract a frame.
  vector<unsigned char> *frame = this->extractFrame(bufferList);

  // Has a frame been extracted?
  if (frame == nullptr) {
    // No frame has been extracted. Return nullptr.
    return shared_ptr<DeviceMessage>();
  }

  // A frame has been extracted. Remove the frame from the original buffer.
  buffer.erase(buffer.begin(), buffer.begin() + frame->size());

  MessageType msgTypeHint;
  return this->decodeFrame(frame, msgTypeHint);
}

vector<unsigned char>
MessageFactory::encodeMessage(shared_ptr<DeviceMessage> msg) {
  Serialization::Messages::DeviceMessageT intermediateObject;

  intermediateObject.sourceId = msg->getSource().id();
  intermediateObject.desinationId = msg->getDestination().id();
  intermediateObject.msgId = msg->getMessageId();

  // Cast down the message and encode the more specific parts.
  MessageType messageType;
  // --------------------------------------------------- Write device message --
  auto writeMsg = dynamic_pointer_cast<WriteDeviceMessage>(msg);
  if (writeMsg) {
    messageType = MessageType::WRITE_DEVICE_MESSAGE;
    Serialization::Messages::WriteDeviceMessageContentT
        writeDeviceMessageContent;
    writeDeviceMessageContent.writeDeviceTopic =
        static_cast<Serialization::Messages::WriteDeviceTopic>(
            writeMsg->getTopic());
    intermediateObject.content.Set(writeDeviceMessageContent);
  } else {

    // ---------------------------------------------------- Handshake message --
    auto handshakeMsg = dynamic_pointer_cast<HandshakeMessage>(msg);
    if (handshakeMsg) {
      messageType = MessageType::HANDSHAKE_MESSAGE;

      Serialization::Messages::HandshakeMessageContentT handshakeMessageContent;
      handshakeMessageContent.version = this->getVersion();
      for (auto statusPayload : handshakeMsg->getPayload()) {
        Serialization::Devices::StatusPayloadT *intermediatePayload =
            new Serialization::Devices::StatusPayloadT();
        intermediatePayload->deviceId = statusPayload->getDeviceId().id();
        intermediatePayload->deviceStatus =
            static_cast<Serialization::Devices::DeviceStatus>(
                statusPayload->getDeviceStatus());
        intermediatePayload->deviceType =
            static_cast<Serialization::Devices::DeviceType>(
                statusPayload->getDeviceType());
        intermediatePayload->deviceName = statusPayload->getDeviceName();
        for (auto proxyId : statusPayload->getProxyIds()) {
          intermediatePayload->proxyIds.push_back(proxyId.id());
        }
        handshakeMessageContent.statusPayloads.emplace_back(
            unique_ptr<Serialization::Devices::StatusPayloadT>(
                intermediatePayload));
      }
      intermediateObject.content.Set(handshakeMessageContent);
    }

    // -------------------------------------------------- Read device message --
    auto readMsg = dynamic_pointer_cast<ReadDeviceMessage>(msg);
    if (readMsg) {
      messageType = MessageType::READ_DEVICE_MESSAGE;
      Serialization::Messages::ReadDeviceMessageContentT
          readDeviceMessageContent;
      readDeviceMessageContent.readDeviceTopic =
          static_cast<Serialization::Messages::ReadDeviceTopic>(
              readMsg->getTopic());
      readDeviceMessageContent.magicNumber =
          readMsg->getReadPaylod()->getMagicNumber();
      readDeviceMessageContent.readPayload = readMsg->getReadPaylod()->bytes();
      intermediateObject.content.Set(readDeviceMessageContent);
    }

    else {
      // ------------------------------------------------ Init device message --
      auto initMsg = dynamic_pointer_cast<InitDeviceMessage>(msg);
      if (initMsg) {
        Serialization::Messages::InitDeviceMessageContentT
            initDeviceMessageContent;
        initDeviceMessageContent.initPayoad = initMsg->returnPayload()->bytes();
        initDeviceMessageContent.magicNumber =
            initMsg->returnPayload()->getMagicNumber();
        intermediateObject.content.Set(initDeviceMessageContent);
      }

      else {
        // -------------------------------------------- Config device message --
        auto configMsg = dynamic_pointer_cast<ConfigDeviceMessage>(msg);
        if (configMsg) {
          Serialization::Messages::ConfigDeviceMessageContentT
              configDeviceMessageContent;
          configDeviceMessageContent.configurationPayload =
              configMsg->getConfiguration()->bytes();
          configDeviceMessageContent.magicNumber =
              configMsg->getConfiguration()->getMagicNumber();
          intermediateObject.content.Set(configDeviceMessageContent);
        }
      }
    }
  }

  // Serialize the intermediate object.
  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Messages::DeviceMessage::Pack(
      builder, &intermediateObject));
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

  // Create a new vector that only contains the flatbuffer payload. flatbuffer
  // will take ownership of this pointer.
  vector<unsigned char> *flatbufferPayload =
      new vector<unsigned char>(++bufferIt, buffer->end() - 1);

  // Decode the payload according to the message type.
  const Serialization::Messages::DeviceMessageT *deviceMsg =
      Serialization::Messages::GetDeviceMessage(flatbufferPayload->data())
          ->UnPack();
  // ------------------------------------------------------ Handshake message --
  if (MessageType::HANDSHAKE_MESSAGE == messageType) {
    return this->translateMessageContent(
        UserId(static_cast<size_t>(deviceMsg->sourceId)),
        UserId(static_cast<size_t>(deviceMsg->desinationId)),
        deviceMsg->content.AsHandshakeMessageContent());
  }
  // --------------------------------------------------- Write device message --
  else if (MessageType::WRITE_DEVICE_MESSAGE == messageType) {
    return this->translateMessageContent(
        UserId(static_cast<size_t>(deviceMsg->sourceId)),
        UserId(static_cast<size_t>(deviceMsg->desinationId)),
        deviceMsg->content.AsWriteDeviceMessageContent(),
        deviceMsg->content.AsWriteDeviceMessageContent()->magicNumber);
  }

  // ---------------------------------------------------- Read Device message --
  else if (MessageType::READ_DEVICE_MESSAGE == messageType) {
    return this->translateMessageContent(
        UserId(static_cast<size_t>(deviceMsg->sourceId)),
        UserId(static_cast<size_t>(deviceMsg->desinationId)),
        deviceMsg->content.AsReadDeviceMessageContent(),
        deviceMsg->content.AsReadDeviceMessageContent()->magicNumber);
  }

  else {
    return shared_ptr<DeviceMessage>();
  }
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::Messages::HandshakeMessageContentT *handshakeContent) {

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
    const Serialization::Messages::WriteDeviceMessageContentT
        *writeDeviceContent,
    int magicNumber) {

  return shared_ptr<WriteDeviceMessage>(new WriteDeviceMessage(
      sourceId, destinationId,
      static_cast<WriteDeviceTopic>(writeDeviceContent->writeDeviceTopic)));
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::Messages::ReadDeviceMessageContentT *readDeviceContent,
    int magicNumber) {

  // Try to parse the payload.
  ReadPayload *decodedPayload =
      this->decodeReadPayload(readDeviceContent->readPayload, magicNumber);
  if (!decodedPayload) {
    // Was not able to decode a read payload from the buffer.
    return shared_ptr<DeviceMessage>();
  }

  return shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(
      sourceId, destinationId,
      static_cast<ReadDeviceTopic>(readDeviceContent->readDeviceTopic),
      decodedPayload, nullptr));
}

ReadPayload *MessageFactory::decodeReadPayload(vector<unsigned char> payload,
                                               int magicNumber) {
  ReadPayload *decodedPayload = nullptr;
  for (auto payloadDecoder : this->payloadDecoders) {
    decodedPayload = payloadDecoder->decodeReadPayload(payload, magicNumber);
    if (decodedPayload != nullptr) {
      break;
    }
  }

  return decodedPayload;
}

WritePayload *MessageFactory::decodeWritePayload(vector<unsigned char> payload,
                                                 int magicNumber) {

  WritePayload *decodedPayload = nullptr;
  for (auto payloadDecoder : this->payloadDecoders) {
    decodedPayload = payloadDecoder->decodeWritePayload(payload, magicNumber);
    if (decodedPayload != nullptr) {
      break;
    }
  }

  return decodedPayload;
}

InitPayload *MessageFactory::decodeInitPayload(vector<unsigned char> payload,
                                               int magicNumber) {

  InitPayload *decodedPayload = nullptr;
  for (auto payloadDecoder : this->payloadDecoders) {
    decodedPayload = payloadDecoder->decodeInitPayload(payload, magicNumber);
    if (decodedPayload != nullptr) {
      break;
    }
  }

  return decodedPayload;
}
ConfigurationPayload *
MessageFactory::decodeConfigurationPayload(vector<unsigned char> payload,
                                           int magicNumber) {

  ConfigurationPayload *decodedPayload = nullptr;
  for (auto payloadDecoder : this->payloadDecoders) {
    decodedPayload = payloadDecoder->decodeConfigPayload(payload, magicNumber);
    if (decodedPayload != nullptr) {
      break;
    }
  }

  return decodedPayload;
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::Messages::InitDeviceMessageContentT *initDeviceContent,
    int magicNumber) {

  // Try to parse the payload.
  InitPayload *decodedPayload =
      this->decodeInitPayload(initDeviceContent->initPayoad, magicNumber);
  if (!decodedPayload) {
    // Was not able to decode a read payload from the buffer.
    return shared_ptr<DeviceMessage>();
  }

  return shared_ptr<InitDeviceMessage>(
      new InitDeviceMessage(sourceId, destinationId, decodedPayload));
}

shared_ptr<DeviceMessage> MessageFactory::translateMessageContent(
    UserId sourceId, UserId destinationId,
    const Serialization::Messages::ConfigDeviceMessageContentT
        *configDeviceContent,
    int magicNumber) {

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