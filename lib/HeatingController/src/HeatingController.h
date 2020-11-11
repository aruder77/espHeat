#ifndef HEATING_CONTROLLER_H_
#define HEATING_CONTROLLER_H_

#include <Module.h>
#include <ArduinoLog.h>
#include <FlowTemperatureRegulator.h>
#include <TargetFlowTemperatureCalculator.h>
#include <ValveController.h>
#include <TemperatureReader.h>
#include <HeatPumpController.h>

class HeatingController : public Module {

    public:
    	static HeatingController* getInstance(); 	
    
        const char *getName();
        void setup();
        void afterSetup();
        void everySecond();
        void every10Milliseconds();        

        void getTelemetryData(char *targetBuffer);

        void setFlowTemperatureSlope(float slope);
        void setFlowTemperatureOrigin(float origin);
        void setPidKp(int kp);
        void setPidTn(int tn);

    private:

        static HeatingController *instance;
        HeatingController();

        HomieNode *heatNode;

        TemperatureReader *temperatureReader;
        FlowTemperatureRegulator *flowTemperatureRegulator;
        TargetFlowTemperatureCalculator *targetFlowTemperatureCalculator;
        ValveController *valveController;
        HeatPumpController *heatPumpController;

        unsigned long timer = 0;
        int loopCounter = 0;

        double flowTempSlope = -0.5;
        double flowTempOrigin = 32.0;
        
        double valveTarget = 0.0;

        double kp = 2.0;
        double tn = 120;

};

#endif