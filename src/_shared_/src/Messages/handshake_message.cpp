// Project includes
#include <handshake_message.hpp>

namespace Messages {

HandshakeMessage::HandshakeMessage(
    UserId source, UserId destination,
    list<shared_ptr<StatusPayload>> statusPayloads, string version)
    : DeviceMessage(source, destination), statusPayloads(statusPayloads),
      version(version) {}

HandshakeMessage::~HandshakeMessage() {}

string HandshakeMessage::serialize() { return ""; }

vector<unsigned char> HandshakeMessage::bytes() {
  return vector<unsigned char>();
}

list<shared_ptr<StatusPayload>> HandshakeMessage::getPayload() {
  return this->statusPayloads;
}

string HandshakeMessage::getVersion() { return this->version; }

} // namespace Messages