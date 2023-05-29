// Project includes
#include <common.hpp>
#include <ob1_conf_payload.hpp>


namespace Devices {

Ob1ConfPayload::Ob1ConfPayload()
    : ConfigurationPayload(DeviceType::PUMP_CONTROLLER) {}

Ob1ConfPayload::~Ob1ConfPayload() {}

int Ob1ConfPayload::getMagicNumber() { return MAGIC_NUMBER_OB1_CONF_PAYLOAD; }
} // namespace Devices