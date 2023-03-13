// Project includes
#include <isx3_init_payload.hpp>

namespace Devices {

Isx3InitPayload::Isx3InitPayload(string ipAddress, int port)
    : ipAddress(ipAddress), port(port) {}

string Isx3InitPayload::serialize() { return ""; }

string Isx3InitPayload::getIpAddress() { return this->ipAddress; }

int Isx3InitPayload::getPort() { return this->port; }

} // namespace Devices