// Standard includes
#include <ostream>
#include <sstream>

// Project includes
#include <common.hpp>
#include <data_response_payload.hpp>

// Generated includes.
#include <data_response_payload_generated.h>

using namespace Devices;
using namespace Utilities;

std::vector<DataResponsePayload *>
DataResponsePayload::constructDataResponsePayload(
    TimePoint from, TimePoint to, std::string key,
    const std::vector<TimePoint> &timestamps,
    const std::vector<Value> &values) {

  std::vector<DataResponsePayload *> retVal;
  bool atEnd = false;
  auto timestampsItBegin = timestamps.begin();
  auto valuesItBegin = values.begin();
  do {
    auto timestampsItEnd =
        std::distance(timestampsItBegin, timestamps.end()) >
                SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH
            ? std::next(timestampsItBegin,
                        SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH)
            : timestamps.end();

    auto valuesItEnd =
        std::distance(valuesItBegin, values.end()) >
                SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH
            ? std::next(valuesItBegin,
                        SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH)
            : values.end();

    std::vector<TimePoint> timepointVec(timestampsItBegin, timestampsItEnd);
    std::vector<Value> valueVec(valuesItBegin, valuesItEnd);

    retVal.emplace_back(new DataResponsePayload(
        from, to, key, timepointVec.size(), timepointVec, valueVec));

    timestampsItBegin = timestampsItEnd;
    valuesItBegin = valuesItEnd;

    atEnd = timestampsItEnd == timestamps.end();

  } while (!atEnd);
  return retVal;
}

DataResponsePayload *DataResponsePayload::constructSingleDataResponsePayload(
    TimePoint from, TimePoint to, std::string key,
    std::vector<TimePoint> &timestamps, std::vector<Value> &values) {

  auto timestampsItBegin = timestamps.begin();
  auto timestampsItEnd =
      timestamps.end() - timestamps.begin() >
              SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH
          ? timestampsItBegin + SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH
          : timestamps.end();

  auto valuesItBegin = values.begin();
  auto valuesItEnd =
      values.end() - values.begin() > SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH
          ? valuesItBegin + SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH
          : values.end();

  std::vector<TimePoint> timepointVec(timestampsItBegin, timestampsItEnd);
  std::vector<Value> valueVec(valuesItBegin, valuesItEnd);

  timestamps.erase(timestampsItBegin, timestampsItEnd);
  values.erase(valuesItBegin, valuesItEnd);

  return new DataResponsePayload(from, to, key, timepointVec.size(),
                                 timepointVec, valueVec);
}

std::string DataResponsePayload::serialize() {
  std::stringstream sstream;

  sstream << "From: " << std::format("{:%Y-%m-%d %H:%M:%S}", this->from)
          << std::endl
          << "To: " << std::format("{:%Y-%m-%d %H:%M:%S}", this->to)
          << std::endl
          << "Key: " << this->key << std::endl
          << "Count: " << this->count << std::endl;

  if (this->count > 0) {
    sstream << "First timestamp: "
            << std::format("{:%Y-%m-%d %H:%M:%S}", this->timestamps.front())
            << std::endl
            << "Last timestamp: "
            << std::format("{:%Y-%m-%d %H:%M:%S}", this->timestamps.back())
            << std::endl;
  }
  return sstream.str();
}

std::vector<unsigned char> DataResponsePayload::DataResponsePayload::bytes() {

  flatbuffers::FlatBufferBuilder builder;

  Serialization::Devices::DataResponsePayloadT intermediateObject;
  intermediateObject.from = this->from.time_since_epoch().count();
  intermediateObject.to = this->to.time_since_epoch().count();
  intermediateObject.key = this->key;
  intermediateObject.count = static_cast<long long>(this->count);

  intermediateObject.timestamps.reserve(intermediateObject.count);
  for (auto timestamp : this->timestamps) {
    intermediateObject.timestamps.push_back(
        timestamp.time_since_epoch().count());
  }

  intermediateObject.values.reserve(intermediateObject.count);
  for (auto &value : this->values) {

    Serialization::Devices::DataResponsePayloadValueUnion valueUnion;
    size_t variantIdx = value.index();
    // int
    if (variantIdx == 0) {
      valueUnion.Set(std::get<int>(value));
    }
    // double
    else if (variantIdx == 1) {
      valueUnion.Set(std::get<double>(value));
    }
    // impedance
    else if (variantIdx == 2) {
      Impedance valueImpedance = std::get<Impedance>(value);
      Serialization::Devices::complex valueComplex{valueImpedance.real(),
                                                   valueImpedance.imag()};
      valueUnion.Set(valueComplex);
    }
    // string
    else if (variantIdx == 3) {
      std::string valueStr = std::get<std::string>(value);
      valueUnion.Set(builder.CreateString(valueStr));
    }
    // spectrum
    else if (variantIdx == 4) {
      ImpedanceSpectrum is = std::get<ImpedanceSpectrum>(value);

      Serialization::Devices::IsPayloadT isPayload;
      isPayload.channelNumber = 0;
      isPayload.timestamp = 0.0;
      std::vector<double> frequencies;
      std::vector<Impedance> impedances;
      Utilities::splitImpedanceSpectrum(is, frequencies, impedances);
      std::vector<Serialization::Devices::complex> complexVec;
      complexVec.reserve(impedances.size());
      for (auto impedance : impedances) {
        complexVec.emplace_back(Serialization::Devices::complex(
            impedance.real(), impedance.imag()));
      }

      isPayload.frequencies = frequencies;
      isPayload.impedances = complexVec;

      valueUnion.Set(isPayload);
    }

    intermediateObject.values.push_back(valueUnion);
  }

  builder.Finish(Serialization::Devices::DataResponsePayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int DataResponsePayload::getMagicNumber() {
  return MAGIC_NUMBER_DATA_RESPONSE_PAYLOAD;
}

DataResponsePayload::DataResponsePayload(
    TimePoint from, TimePoint to, const std::string &key, size_t count,
    const std::vector<TimePoint> &timestamps, const std::vector<Value> &values)
    : from(from), to(to), key(key), count(count), timestamps(timestamps),
      values(values) {}
