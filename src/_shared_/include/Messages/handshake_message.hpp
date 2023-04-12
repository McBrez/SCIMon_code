#ifndef HANDHSAKE_MESSAGE_HPP
#define HANDHSAKE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>
#include <status_payload.hpp>

using namespace Devices;

namespace Messages {
class HandshakeMessage : public DeviceMessage {
public:
  HandshakeMessage(UserId source, UserId destination,
                   list<shared_ptr<StatusPayload>> statusPayloads);

  virtual ~HandshakeMessage() override;

  virtual string serialize() override;

  virtual vector<unsigned char> bytes();

  list<shared_ptr<StatusPayload>> getPayload();

private:
  list<shared_ptr<StatusPayload>> statusPayloads;
};
} // namespace Messages

#endif