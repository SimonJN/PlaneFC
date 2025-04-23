#include <Arduino.h>

class MotorDriver
{
private:
    // Settings (currently hardcoded)
    int duty_cycle_change_step = 100;
    const int pwm_channel = 0;     // Use PWM channel 0
    const int pwm_frequency = 500; // 500 Hz frequency for the ESC
    const int pwm_resolution = 16; // 16-bit resolution
    const int pwm_pin = 5;         // Pin connected to the ESC signal wire

    // State variables
    bool failsafe_active = false;
    bool armed = false;
    bool soft_started = false;

    int goal_duty_cycle = 1000;
    int current_duty_cycle = 1000;

    TaskHandle_t increaser_handle;

    void smoothSpeedChanger();
    static void dispatcher(void *pvParameters);

    int dutyCycleToPWM(int duty_cycle);

public:
    MotorDriver();
    ~MotorDriver();

    void setGoalDutyCycle(int goal);
    void setArmed(bool is_armed);
    void setFailsafeActive(bool failsafe_is_active);
};