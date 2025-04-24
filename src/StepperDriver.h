#include "MotorDriver.h"

class StepperDriver : public MotorDriver
{
private:
    void smoothDutyChanger() override;

public:
    StepperDriver(int pin, int channel, int change_step, const char *name);
    ~StepperDriver();
};
