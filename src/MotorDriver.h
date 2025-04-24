#include <Arduino.h>

class MotorDriver
{
private:
    TaskHandle_t duty_changer_handle;

    virtual void smoothDutyChanger();
    static void dispatcher(void *pvParameters);

protected:
    // Settings
    int duty_cycle_change_step; // How much the duty cycle can be changed by in one time-step
    int pwm_channel;            // Use PWM channel 0
    int pwm_pin;                // Pin connected to the signal wire

    int pwm_frequency = 500; // 500 Hz frequency for the PWM signal
    int pwm_resolution = 16; // 16-bit resolution

    // State variables
    int goal_duty_cycle = 1000;
    int current_duty_cycle = 1000;

    int dutyCycleToPWM(int duty_cycle);

public:
    MotorDriver(int pin, int channel, int change_step);
    ~MotorDriver();

    virtual void setGoalDutyCycle(int goal);
};