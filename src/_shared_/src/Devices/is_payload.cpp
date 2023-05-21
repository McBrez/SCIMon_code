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
                     list<double> frequencies, list<complex<double>> impedances)
    : channelNumber(channelNumber), timestamp(timestamp) {

  // Get the smaller of the both lists.
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

string IsPayload::serialize() {
  string retVal;
  retVal += "timestamp: " + to_string(this->timestamp) + " ";
  retVal += "channel: " + to_string(this->channelNumber) + "\n";
  for (auto impedancePoint : this->impedanceSpectrum) {
    retVal += to_string(get<0>(impedancePoint)) + " Hz, " +
              to_string(get<1>(impedancePoint).real()) + "Ohm + i * " +
              to_string(get<1>(impedancePoint).imag()) + " Ohm";
  }

  return retVal;
}

vector<unsigned char> IsPayload::bytes() {
  Serialization::IsPayloadT intermediateObject;
  intermediateObject.channelNumber = this->channelNumber;
  intermediateObject.timestamp = this->timestamp;
  vector<double> frequencies;
  vector<Impedance> impedances;
  Utilities::splitImpedanceSpectrum(this->impedanceSpectrum, frequencies,
                                    impedances);
  intermediateObject.frequencies = frequencies;
  for (auto impedance : impedances) {
    intermediateObject.impedances.emplace_back(
        Serialization::complex(impedance.real(), impedance.imag()));
  }

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::IsPayload::Pack(builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return vector<unsigned char>(buffer, buffer + builder.GetSize());
}

} // namespace Devices