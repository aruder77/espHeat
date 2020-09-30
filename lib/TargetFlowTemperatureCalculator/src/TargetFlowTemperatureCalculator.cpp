#include "TargetFlowTemperatureCalculator.h"

double TargetFlowTemperatureCalculator::calculateTargetFlowTemperature(double outsideTemperature) {
    return min(origin + outsideTemperature * slope, maxFlowTemp);
}

void TargetFlowTemperatureCalculator::setFlowTemperatureSlope(double s) {
    slope = s;
}

void TargetFlowTemperatureCalculator::setFlowTemperatureOrigin(double o) {
    origin = o;
}