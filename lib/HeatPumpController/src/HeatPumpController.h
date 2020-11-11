#ifndef HeatPumpController_H_
#define HeatPumpController_H_

#include <Arduino.h>
#include <Homie.h>

class HeatPumpController {
    public:
        HeatPumpController();
        void on();
        void off();

    private:
        uint8_t pumpPin = 5;
};

#endif