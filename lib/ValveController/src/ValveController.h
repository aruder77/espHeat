#ifndef VALVECONTROLLER_H_
#define VALVECONTROLLER_H_

#include <math.h>
#include <Arduino.h>
#include <Homie.h>

class ValveController {

    public:
        ValveController();
        void setup();
        void every10Milliseconds();
        void setTargetValvePosition(int targetValvePosition);
        int getValveCurrent();
        int getValveTarget();
        void adjustTargetValvePosition();

    private:
        static const int VALVE_ONE_PERCENT_OPEN_CYCLES = 55;

        uint8_t openPin = 19;
        uint8_t closePin = 18;

        int motorAdjustCounter = 0;

        int valveCurrent = 0;
        int valveTarget = 0;
        int tempValveTarget = 0;

        int8_t valveState = 0;
};

#endif