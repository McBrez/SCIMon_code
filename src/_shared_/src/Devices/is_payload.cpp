// Project includes
#include <is_payload.hpp>
#include <utilities.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <is_payload_generated.h>

namespace Devices {

IsPayload::IsPayload(unsigned int channelNumber, double timestamp,
                     ImpedanceSpectrum impedanceSpectrum)
    : channelNumber(channelNumber), timestamp(timestamp),
      impedanceSpectrum(impedanceSpectrum) {}

IsPayload::IsPayload(unsigned int channelNumber, double timestamp,
                     std::list<double> frequencies,
                     std::list<std::complex<double>> impedances)
    : channelNumber(channelNumber), timestamp(timestamp) {

  // Get the smaller of the both std::lists.
  size_t minLen = std::min(frequencies.size(), impedances.size());
  auto frequenciesIt = frequencies.begin();
  auto impedancesIt = impedances.begin();
  for (size_t i = 0; i < minLen; i++) {
    this->impedanceSpectrum.push_back(
        ImpedancePoint(*frequenciesIt, *impedancesIt));
    ++frequenciesIt;
    ++impedancesIt;
  }
}

unsigned int IsPayload::getChannelNumber() const { return this->channelNumber; }

double IsPayload::getTimestamp() const { return this->timestamp; }

ImpedanceSpectrum IsPayload::getImpedanceSpectrum() const {
  return this->impedanceSpectrum;
}

std::string IsPayload::serialize() {
  std::string retVal;
  retVal += "timestamp: " + std::to_string(this->timestamp) + " ";
  retVal += "channel: " + std::to_string(this->channelNumber) + "\n";
  for (auto impedancePoint : this->impedanceSpectrum) {
    retVal += std::to_string(get<0>(impedancePoint)) + " Hz, " +
              std::to_string(get<1>(impedancePoint).real()) + "Ohm + i * " +
              std::to_string(get<1>(impedancePoint).imag()) + " Ohm";
  }

  return retVal;
}

std::vector<unsigned char> IsPayload::bytes() {
  Serialization::Devices::IsPayloadT intermediateObject;
  intermediateObject.channelNumber = this->channelNumber;
  intermediateObject.timestamp = this->timestamp;
  std::vector<double> frequencies;
  std::vector<Impedance> impedances;
  Utilities::splitImpedanceSpectrum(this->impedanceSpectrum, frequencies,
                                    impedances);
  intermediateObject.frequencies = frequencies;
  for (auto impedance : impedances) {
    intermediateObject.impedances.emplace_back(
        Serialization::Devices::complex(impedance.real(), impedance.imag()));
  }

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(
      Serialization::Devices::IsPayload::Pack(builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int IsPayload::getMagicNumber() { return MAGIC_NUMBER_IS_PAYLOAD; }

} // namespace Devices