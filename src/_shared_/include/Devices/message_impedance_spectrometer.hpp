#ifndef MESSAGE_IMPEDANCE_SPECTROMETER_HPP
#define MESSAGE_IMPEDANCE_SPECTROMETER_HPP

#include <device_message.hpp>

using namespace std;

namespace Devices
{
    /**
     * @brief Identifies the different impedance spectrometer message types.
     *
     */
    enum ImpedanceSpectrometerMessageType
    {
        INVALID_MSG = -1,
        INITIALIZE = 1,
        RESET,
        CALIBRATE,
        START_MEASUREMENT,
        STOP_MEASUREMENT,
        GET_DATA
    };

    class MessageImpedanceSpectrometer : public DeviceMessage
    {
    public:
        /**
         * @brief Construct a new Message Impedance Spectrometer object
         *
         * @param messageType The type of the message.
         */
        MessageImpedanceSpectrometer(ImpedanceSpectrometerMessageType messageType);

        /**
         * @brief Returns the type of the message.
         *
         * @return The type of the message.e
         */
        ImpedanceSpectrometerMessageType type();

        /**
         * @brief Serializes the contents of the message into a string.
         *
         * @return The string representation of the message.
         */
        virtual string serialize() override;

    private:
        /// The message type.
        ImpedanceSpectrometerMessageType messageType;
    };
}

#endif