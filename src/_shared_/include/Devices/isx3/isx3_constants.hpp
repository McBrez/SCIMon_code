#ifndef ISX3_CONSTANTS_HPP
#define ISX3_CONSTANTS_HPP

// The product id of the sciospec ISX3.
#define ISX3_PID 0x89D0

namespace Devices {

// const string Isx3DeviceTypeName = "Sciospec ISX-3";

/**
 * @brief Identifies the various ISX3 command tags.
 */
enum Isx3CmdTag {
  /// Identifies an invalid command tag.
  ISX3_COMMAND_TAG_INVALID = 0xFF,
  /// Identifies an acknowlegment frame.
  ISX3_COMMAND_TAG_ACK = 0x18,
  /// Command tag for the "Reset System" command.
  ISX3_COMMAND_TAG_RESET_SYSTEM = 0xA1,
  /// Command tag for the "Set FE Settings" command.
  ISX3_COMMAND_TAG_SET_FE_SETTINGS = 0xB0,
  /// Command tag for the "Set ExtensionPort Channel" command.
  ISX3_COMMAND_TAG_SET_EXTENSIONPORT_CHANNEL = 0xB2,
  /// Command tag for the "Get Device Id" command.
  ISX3_COMMAND_GET_DEVICE_ID = 0xD1,
  /// Command tag for an "Init Measurement" command.
  ISX3_COMMAND_TAG_INIT_MEASUREMENT = 0xB4,
  /// Command tag for an "Set Setup" command.
  ISX3_COMMAND_TAG_SET_SETUP = 0xB6,
  /// Command tag for an "Start Impedance Measurement" command. Also used for
  /// measurement results sent back by the device.
  ISX3_COMMAND_TAG_START_IMPEDANCE_MEAS = 0xB8
};

/**
 * @brief Identifies the various ISX3 Acknowledgment types.
 */
enum Isx3AckType {
  /// Invalid ack type. May be used to indicate a not yet received ack.
  ISX3_ACK_TYPE_INVALID = 0xFF,
  /// Frame-Not-Acknowledge: Incorrect syntax.
  ISX3_ACK_TYPE_FRAME_NOT_ACKNOWLEDGED = 0x01,
  /// Timeout: Communication-timeout (less data than expected)
  ISX3_ACK_TYPE_TIMEOUT = 0x02,
  /// Wake-up Message: System boot read.
  ISX3_ACK_TYPE_WAKEUP = 0x04,
  /// TCP-Socket: Valid TCP client-socket connection
  ISX3_ACK_TYPE_TCP_SOCKET = 0x11,
  /// Not-Acknowledge: Command has not been executed
  ISX3_ACK_TYPE_COMMAND_NOT_ACKNOWLEGDE = 0x81,
  /// Not-Acknowledge: Command could not be recognized
  ISX3_ACK_TYPE_COMMAND_NOT_RECOGNIZED = 0x82,
  /// Command-Acknowledge: Command has been executed successfully
  ISX3_ACK_TYPE_COMMAND_ACKNOWLEDGE = 0x83,
  /// System-Ready Message: System is operational and ready to receive data
  ISX3_ACK_TYPE_SYSTEM_READY = 0x84,
  /// Data holdup: Measurement data could not be sent via the master interface
  ISX3_ACK_TYPE_DATA_HOLDUP = 0x92
};

/**
 * @brief Identifies measurements modes as used by the init measurement command.
 */
enum MeasurementMode {
  MEASUREMENT_MODE_INVALID = 0xFF,
  MEASUREMENT_MODE_EFP = 0x00,
  MEASUREMENT_MODE_EFP_STIMULATION = 0x01,
  MEASUREMENT_MODE_IMPEDANCE_SPECTROSCOPY = 0x02,
  MEASUREMENT_MODE_FULL_RANGE_IMPEDANCE_SPECTROSCOPY = 0x03
};

/**
 * @brief Identifies the command options of the "Set Setup" command.
 */
enum SetSetupCommandOption {
  /// Invalid command option
  SET_SETUP_CMD_OPTION_INVALID = 0xFF,
  /// Initializes the measurement setup.
  SET_SETUP_CMD_OPTION_INIT_MEASUREMENT = 0x01,
  /// Adds a specific frequency point to the measurement setup.
  SET_SETUP_CMD_OPTION_ADD_FREQ_POINT = 0x02,
  /// Adds a list of frequencies to the measurement setup.
  SET_SETUP_CMD_OPTION_ADD_FREQ_LIST = 0x03
};

/**
 * @brief Identifies the possible scalings of the frequency axis.
 */
enum FrequencyScale {
  /// Invalid scaling.
  FREQ_SCALE_INVALID = 0xFF,
  /// Linear axis scaling.
  FREQ_SCALE_LINEAR = 0x00,
  /// Logarithmic axis scaling.
  FREQ_SCALE_LOGARITHMIC = 0x01
};

/**
 * @brief Identifies the possible measurement configurations.
 */
enum MeasurementConfiguration {
  MEAS_CONFIG_INVALID = 0xFF,

  MEAS_CONFIG_2_POINT = 0x01,

  MEAS_CONFIG_3_POINT = 0x03,

  MEAS_CONFIG_4_POINT = 0x02,
};

/**
 * @brief Identifies the possible front end channel configuration.
 */
enum MeasurmentConfigurationChannel {
  MEAS_CONFIG_CHANNEL_INVALID = 0xFF,

  MEAS_CONFIG_CHANNEL_BNC = 0x01,

  MEAS_CONFIG_CHANNEL_EXT_PORT = 0x02,

  MEAS_CONFIG_CHANNEL_EXT_PORT_2 = 0x03
};

/**
 * @brief Identifies the possible front end measurement ranges.
 */
enum MeasurmentConfigurationRange {
  MEAS_CONFIG_RANGE_INVALID = 0xFF,

  MEAS_CONFIG_RANGE_10MA = 0x01,

  MEAS_CONFIG_RANGE_100UA = 0x02,

  MEAS_CONFIG_RANGE_1UA = 0x04,

  MEAS_CONFIG_RANGE_10NA = 0x06
};

} // namespace Devices

#endif
