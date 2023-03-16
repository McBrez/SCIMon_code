// Project includes
#include <isx3_command_buffer.hpp>
#include <isx3_constants.hpp>

namespace Devices {

Isx3CommandBuffer::Isx3CommandBuffer(unsigned int bufferSize)
    : bufferSize(bufferSize) {}

void Isx3CommandBuffer::pushBytes(const std::vector<unsigned char> &bytes) {
  for (auto byteValue : bytes) {
    if (this->bufferSize != 0 && this->buffer.size() >= this->bufferSize) {
      this->buffer.pop_front();
    }
    this->buffer.push_back(byteValue);
  }
}

std::vector<unsigned char> Isx3CommandBuffer::interpretBuffer() {

  while (true) {
    // Search for the next command tag.
    unsigned char cmdTag = 0x00;
    bool cmdTagFound = false;
    while (this->buffer.size() > 0 && !cmdTagFound) {
      if (this->isCmdTag(this->buffer.front())) {
        cmdTag = buffer.front();
        cmdTagFound = true;
      } else {
        buffer.pop_front();
      }
    }
    if (!cmdTagFound) {
      return std::vector<unsigned char>();
    }

    // Opening command tag detected. The next byte should contain the length of
    // the payload. Look ahead and find the closing command tag.
    // Note: The smallest frame is 3 bytes. Anything shorter than that cannot be
    // a valid frame.
    if (this->buffer.size() < 3) {
      return std::vector<unsigned char>();
    }
    auto bufferIt = this->buffer.begin();
    unsigned char len = *(++bufferIt);
    if (len > this->buffer.size() - 1) {
      // Buffer ends prematurely. It might be the case that the closing command
      // tag has not been received yet. Return here and wait until more bytes
      // have been received.
      return std::vector<unsigned char>();
    }
    std::advance(bufferIt, len + 1);
    if (*bufferIt != cmdTag) {
      // Closing command tag not found. The opening command tag did not start a
      // frame. Remove it from the buffer and start interpretation again.
      this->buffer.pop_front();
    } else {
      // Found closing command tag. Remove the frame from the buffer and return.
      std::vector<unsigned char> extractedFrame(this->buffer.begin(),
                                                ++bufferIt);
      this->buffer.erase(this->buffer.begin(), bufferIt);
      return extractedFrame;
    }
  }
}

unsigned int Isx3CommandBuffer::clear() {
  unsigned int size = this->buffer.size();
  this->buffer.clear();

  return size;
}

bool Isx3CommandBuffer::isCmdTag(unsigned char byteValue) {
  if (Isx3CmdTag::ISX3_COMMAND_TAG_INVALID == byteValue ||
      Isx3CmdTag::ISX3_COMMAND_TAG_ACK == byteValue ||
      Isx3CmdTag::ISX3_COMMAND_TAG_RESET_SYSTEM == byteValue ||
      Isx3CmdTag::ISX3_COMMAND_GET_DEVICE_ID == byteValue ||
      Isx3CmdTag::ISX3_COMMAND_TAG_INIT_MEASUREMENT == byteValue ||
      Isx3CmdTag::ISX3_COMMAND_TAG_SET_SETUP == byteValue ||
      Isx3CmdTag::ISX3_COMMAND_TAG_START_IMPEDANCE_MEAS == byteValue) {
    return true;
  } else {
    return false;
  }
}

} // namespace Devices