#ifndef ISX3_CMD_ACK_STRUCT_HPP
#define ISX3_CMD_ACK_STRUCT_HPP

// Standard includes
#include <functional>

// Project includes
#include <isx3_constants.hpp>

namespace Devices {
/**
 * Structure that holds the information of a sent data frame.
 */
struct Isx3CmdAckStruct {
  /// Unix timestamp of when the command has been sent.
  long timestamp;
  /// The command tag of the sent frame.
  Isx3CmdTag cmdTag;
  /// Whether the command has already been acknowledged.
  Isx3AckType acked;
  /// Whether this object is still cached.
  bool cached;

  bool operator==(const Isx3CmdAckStruct &other) {
    if (this->timestamp == other.timestamp && this->cmdTag == other.cmdTag) {
      return true;
    } else {
      return false;
    }
  }
};

} // namespace Devices

#endif
