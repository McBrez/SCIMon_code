#ifndef MAIN_CONTROL_STATE_HPP
#define MAIN_CONTROL_STATE_HPP

namespace Control
{
    enum MainControlState
    {
        INVALID_CONTROL_STATE = -1,
        INIT,
        CALIBRATION,
        IDLE,
        RUNNING
    };

    /**
     * @brief Encapsulates a state machine that implements a control logic.
     */
    class MainControl
    {
    private:
        MainControlState state;

    public:
        MainControl();
        void run();
    };
}

#endif