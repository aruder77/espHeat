#ifndef TARGETFLOWTEMPERATURECALCULATOR_H_
#define TARGETFLOWTEMPERATURECALCULATOR_H_

#include <Arduino.h>

class TargetFlowTemperatureCalculator {
    public:
        double calculateTargetFlowTemperature(double outsideTemperature);

        void setFlowTemperatureSlope(double slope);
        void setFlowTemperatureOrigin(double origin);

    private:
        double slope = -0.3;
        double origin = 35.0;
        double maxFlowTemp = 40.0;
};

#endif
