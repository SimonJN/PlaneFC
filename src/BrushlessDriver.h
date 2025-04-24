#include "MotorDriver.h"

class BrushlessDriver : public MotorDriver
{
private:
    // State variables
    bool failsafe_active = false;
    bool armed = false;
    bool soft_started = false;

    void smoothDutyChanger() override;
    
public:
    BrushlessDriver();
    ~BrushlessDriver();

    void setGoalDutyCycle(int goal) override;

    void setArmed(bool is_armed);
    void setFailsafeActive(bool failsafe_is_active);
};
