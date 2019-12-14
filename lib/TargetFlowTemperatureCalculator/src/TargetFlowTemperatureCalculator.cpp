#include "TargetFlowTemperatureCalculator.h"

double TargetFlowTemperatureCalculator::calculateTargetFlowTemperature(double outsideTemperature) {
    return this->origin + outsideTemperature * slope;
}

void TargetFlowTemperatureCalculator::setFlowTemperatureSlope(double slope) {
    this->slope = slope;
}

void TargetFlowTemperatureCalculator::setFlowTemperatureOrigin(double origin) {
    this->origin = origin;
}