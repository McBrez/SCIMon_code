// Project includes
#include <read_payload.hpp>

namespace Devices {

ReadPayload::ReadPayload() : additionalData("") {}

ReadPayload::~ReadPayload() {}

string ReadPayload::getAdditionalData() const { return this->additionalData; }

void ReadPayload::setAdditionalData(const string &additionalData) {
  this->additionalData = additionalData;
}

} // namespace Devices
