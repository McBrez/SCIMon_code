#ifndef ISX3_CONSTANTS_HPP
#define ISX3_CONSTANTS_HPP

// The product id of the sciospec ISX3.
#define ISX3_PID 0x89D0

namespace Devices {

enum Isx3CmdType {
  ISX3_COMMAND_TAG_ACK = 0x18,
  ISX3_COMMAND_TAG_SET_SETUP = 0xB6
};

enum Isx3AckType {
  ISX3_ACK_TYPE_FRAME_NOT_ACKNOWLEDGED = 0x01,
  ISX3_ACK_TYPE_COMMAND_ACKNOWLEGDE = 0x81
};
} // namespace Devices

#endif