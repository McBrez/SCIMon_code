// Project includes
#include <handshake_message.hpp>

namespace Messages {

HandshakeMessage::HandshakeMessage(
    UserId source, UserId destination,
    list<shared_ptr<StatusPayload>> statusPayloads)
    : DeviceMessage(source, destination), statusPayloads(statusPayloads) {}

HandshakeMessage::~HandshakeMessage() {}

string HandshakeMessage::serialize() { return ""; }

vector<unsigned char> HandshakeMessage::bytes() {
  return vector<unsigned char>();
}

list<shared_ptr<StatusPayload>> HandshakeMessage::getPayload() {
  return this->statusPayloads;
}

} // namespace Messages