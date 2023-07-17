// Project includes
#include <builtin_payload_decoder.hpp>
#include <common.hpp>
#include <data_response_payload.hpp>>
#include <generic_read_payload.hpp>
#include <request_data_payload.hpp>
#include <set_device_status_payload.hpp>
#include <set_pressure_payload.hpp>
#include <status_payload.hpp>
#include <user_id.hpp>

// Generated includes
#include <data_response_payload_generated.h>>
#include <generic_read_payload_generated.h>
#include <is_payload_generated.h>
#include <request_data_payload_generated.h>
#include <set_device_state_payload_generated.h>
#include <set_pressure_payload_generated.h>
#include <status_payload_generated.h>

using namespace Devices;
using namespace Messages;

InitPayload *
BuiltinPayloadDecoder::decodeInitPayload(const std::vector<unsigned char> &data,
                                         int magicNumber) {
  // No init payloads are built-in.
  return nullptr;
}

ConfigurationPayload *BuiltinPayloadDecoder::decodeConfigPayload(
    const std::vector<unsigned char> &data, int magicNumber) {
  // No config payloads are built-in.
  return nullptr;
}

ReadPayload *
BuiltinPayloadDecoder::decodeReadPayload(const std::vector<unsigned char> &data,
                                         int magicNumber) {
  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_GENERIC_READ_PAYLOAD == magicNumber) {
    const Serialization::Devices::GenericReadPayload *genericReadPayload =
        Serialization::Devices::GetGenericReadPayload(buffer);
    std::vector<unsigned char> byteVector(
        genericReadPayload->byteVector()->begin(),
        genericReadPayload->byteVector()->end());

    return new GenericReadPayload(byteVector);
  }

  else if (MAGIC_NUMBER_STATUS_PAYLOAD == magicNumber) {
    const Serialization::Devices::StatusPayloadT *statusPayload =
        Serialization::Devices::GetStatusPayload(buffer)->UnPack();

    UserId deviceId(statusPayload->deviceId);
    DeviceStatus deviceStatus =
        static_cast<DeviceStatus>(statusPayload->deviceStatus);
    std::list<UserId> proxyIds;
    for (auto proxyIdRaw : statusPayload->proxyIds) {
      proxyIds.emplace_back(UserId(proxyIdRaw));
    }
    DeviceType deviceType = static_cast<DeviceType>(statusPayload->deviceType);
    std::string deviceName = statusPayload->deviceName;

    return new StatusPayload(deviceId, deviceStatus, proxyIds, deviceType,
                             deviceName);
  }

  else if (MAGIC_NUMBER_DATA_RESPONSE_PAYLOAD == magicNumber) {
    const Serialization::Devices::DataResponsePayloadT *dataResponsePayload =
        Serialization::Devices::GetDataResponsePayload(buffer)->UnPack();

    TimePoint from =
        TimePoint(std::chrono::milliseconds(dataResponsePayload->from));
    TimePoint to =
        TimePoint(std::chrono::milliseconds(dataResponsePayload->to));
    std::string key = dataResponsePayload->key;
    size_t count = dataResponsePayload->count;
    std::vector<TimePoint> timestamps;
    timestamps.reserve(dataResponsePayload->count);
    for (auto timestamp : dataResponsePayload->timestamps) {
      timestamps.push_back(TimePoint(std::chrono::milliseconds(timestamp)));
    }

    std::vector<Value> values;
    values.reserve(count);
    for (auto valueUnion : dataResponsePayload->values) {
      Value value;
      if (Serialization::Devices::DataResponsePayloadValue::
              DataResponsePayloadValue_DataResponsePayloadValueInt ==
          valueUnion.type) {
        value = Value(valueUnion.AsDataResponsePayloadValueInt()->value);
      } else if (Serialization::Devices::DataResponsePayloadValue::
                     DataResponsePayloadValue_DataResponsePayloadValueFloat ==
                 valueUnion.type) {
        value = Value(valueUnion.AsDataResponsePayloadValueFloat()->value);
      } else if (Serialization::Devices::DataResponsePayloadValue::
                     DataResponsePayloadValue_DataResponsePayloadValueString ==
                 valueUnion.type) {
        value = Value(valueUnion.AsDataResponsePayloadValueString()->value);
      } else if (Serialization::Devices::DataResponsePayloadValue::
                     DataResponsePayloadValue_complex == valueUnion.type) {

        Serialization::Devices::complex *valueComplex = valueUnion.Ascomplex();
        value =
            Value(Impedance(valueComplex->real(), valueComplex->imaginary()));
      } else if (Serialization::Devices::DataResponsePayloadValue::
                     DataResponsePayloadValue_IsPayload == valueUnion.type) {

        Serialization::Devices::IsPayloadT *isPayload =
            valueUnion.AsIsPayload();
        std::vector<double> frequencies = isPayload->frequencies;
        std::vector<Serialization::Devices::complex> complexValues =
            isPayload->impedances;
        std::vector<Impedance> impedances;
        impedances.reserve(complexValues.size());
        for (auto &complexValue : complexValues) {
          impedances.emplace_back(
              Impedance(complexValue.real(), complexValue.imaginary()));
        }

        ImpedanceSpectrum impedanceSpectrum;
        Utilities::joinImpedanceSpectrum(frequencies, impedances,
                                         impedanceSpectrum);

        value = Value(impedanceSpectrum);
      }

      values.push_back(value);
    }

    return DataResponsePayload::constructSingleDataResponsePayload(
        from, to, key, timestamps, values);
  }

  else {
    return nullptr;
  }
}

WritePayload *BuiltinPayloadDecoder::decodeWritePayload(
    const std::vector<unsigned char> &data, int magicNumber) {

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
        TimePoint(std::chrono::milliseconds(requestDataPayload->from)),
        TimePoint(std::chrono::milliseconds(requestDataPayload->to)),
        requestDataPayload->key);
  }

  else if (MAGIC_NUMBER_SET_DEVICE_STATUS_PAYLOAD == magicNumber) {
    const Serialization::Devices::SetDeviceStatePayloadT
        *setDeviceStatePayload =
            Serialization::Devices::GetSetDeviceStatePayload(buffer)->UnPack();

    return new SetDeviceStatusPayload(setDeviceStatePayload->setStatus,
                                      setDeviceStatePayload->targetDeviceId);
  }

  else {
    return nullptr;
  }
}
