// Project includes
#include <builtin_payload_decoder.hpp>
#include <common.hpp>
#include <generic_read_payload.hpp>
#include <request_data_payload.hpp>
#include <set_pressure_payload.hpp>
#include <status_payload.hpp>
#include <user_id.hpp>

// Generated includes
#include <generic_read_payload_generated.h>
#include <is_payload_generated.h>
#include <request_data_payload_generated.h>
#include <set_pressure_payload_generated.h>
#include <status_payload_generated.h>

using namespace Devices;
using namespace Messages;
using namespace std;

InitPayload *
BuiltinPayloadDecoder::decodeInitPayload(const vector<unsigned char> &data,
                                         int magicNumber) {
  // No init payloads are built-in.
  return nullptr;
}

ConfigurationPayload *
BuiltinPayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data,
                                           int magicNumber) {
  // No config payloads are built-in.
  return nullptr;
}

ReadPayload *
BuiltinPayloadDecoder::decodeReadPayload(const vector<unsigned char> &data,
                                         int magicNumber) {
  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_GENERIC_READ_PAYLOAD == magicNumber) {
    const Serialization::Devices::GenericReadPayload *genericReadPayload =
        Serialization::Devices::GetGenericReadPayload(buffer);
    vector<unsigned char> byteVector(genericReadPayload->byteVector()->begin(),
                                     genericReadPayload->byteVector()->end());

    return new GenericReadPayload(byteVector);
  }

  else if (MAGIC_NUMBER_STATUS_PAYLOAD == magicNumber) {
    const Serialization::Devices::StatusPayloadT *statusPayload =
        Serialization::Devices::GetStatusPayload(buffer)->UnPack();

    UserId deviceId(statusPayload->deviceId);
    DeviceStatus deviceStatus =
        static_cast<DeviceStatus>(statusPayload->deviceStatus);
    list<UserId> proxyIds;
    for (auto proxyIdRaw : statusPayload->proxyIds) {
      proxyIds.emplace_back(UserId(proxyIdRaw));
    }
    DeviceType deviceType = static_cast<DeviceType>(statusPayload->deviceType);
    string deviceName = statusPayload->deviceName;

    return new StatusPayload(deviceId, deviceStatus, proxyIds, deviceType,
                             deviceName);
  } else {
    return nullptr;
  }
}

WritePayload *
BuiltinPayloadDecoder::decodeWritePayload(const vector<unsigned char> &data,
                                          int magicNumber) {

  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_SET_PRESSURE_PAYLOAD == magicNumber) {
    const Serialization::Devices::SetPressurePayloadT *setPressurePayload =
        Serialization::Devices::GetSetPressurePayload(buffer)->UnPack();

    return new SetPressurePayload(
        setPressurePayload->pressures,
        static_cast<PressureUnit>(setPressurePayload->pressureUnit));
  }

  else if (MAGIC_NUMBER_REQUEST_DATA_PAYLOAD == magicNumber) {
    const Serialization::Devices::RequestDataPayloadT *requestDataPayload =
        Serialization::Devices::GetRequestDataPayload(buffer)->UnPack();

    return new RequestDataPayload(
        static_cast<DeviceType>(requestDataPayload->deviceType),
        TimePoint(chrono::milliseconds(requestDataPayload->from)),
        TimePoint(chrono::milliseconds(requestDataPayload->to)));
  }

  else {
    return nullptr;
  }
}