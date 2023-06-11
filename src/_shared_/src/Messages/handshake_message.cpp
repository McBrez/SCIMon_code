// Project includes
#include <handshake_message.hpp>

namespace Messages {

HandshakeMessage::HandshakeMessage(
    UserId source, UserId destination,
    std::list<std::shared_ptr<StatusPayload>> statusPayloads,
    std::string version)
    : DeviceMessage(source, destination), statusPayloads(statusPayloads),
      version(version) {}

HandshakeMessage::~HandshakeMessage() {}

std::string HandshakeMessage::serialize() { return ""; }

std::vector<unsigned char> HandshakeMessage::bytes() {
  return std::vector<unsigned char>();
}

std::list<std::shared_ptr<StatusPayload>> HandshakeMessage::getPayload() {
  return this->statusPayloads;
}

std::string HandshakeMessage::getVersion() { return this->version; }

} // namespace Messages