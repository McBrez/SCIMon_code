#ifndef ISX3_COMMAND_BUFFER
#define ISX3_COMMAND_BUFFER

// Standard include
#include <list>
#include <vector>

namespace Devices {
/**
 * @brief Provides a class extracts Sciospec COM interface frames from a buffer.
 */
class Isx3CommandBuffer {
public:
  /**
   * @brief Construct a new Isx 3 Command Buffer object
   * @param bufferSize The size of the internal buffer. If 0, the buffer has
   * unliminted size.
   */
  Isx3CommandBuffer(unsigned int bufferSize = 0);

  /**
   * @brief Pushes the given bytes to the buffer.
   * @param bytes The bytes that shall be put into the buffer.
   */
  void pushBytes(const std::vector<unsigned char> &bytes);

  /**
   * @brief Interprets the buffer and tries to extract a data frame from it.
   * @return Will contain a data frame if one has been found. Empty vector if
   * none has been discovered.
   */
  std::vector<unsigned char> interpretBuffer();

  /**
   * @brief Clears the internal buffer.
   * @return Count of bytes that have been cleared.
   */
  unsigned int clear();

  /**
   * @brief Returns whether the given byte is a command tag.
   * @param byte The byte that shall be analysed
   * @return TRUE if the given byte is a command tag. FALSE otherwise.
   */
  bool isCmdTag(unsigned char byte);

private:
  /// Internal byte buffer.
  std::list<unsigned char> buffer;

  /// The maximum buffer size.
  unsigned int bufferSize;
};
} // namespace Devices

#endif