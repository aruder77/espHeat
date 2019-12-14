#ifndef TARGETFLOWTEMPERATURECALCULATOR_H_
#define TARGETFLOWTEMPERATURECALCULATOR_H_

class TargetFlowTemperatureCalculator {
    public:
        double calculateTargetFlowTemperature(double outsideTemperature);

        void setFlowTemperatureSlope(double slope);
        void setFlowTemperatureOrigin(double origin);

    private:
        double slope = -0.5;
        double origin = 30.0;
};

#endif
