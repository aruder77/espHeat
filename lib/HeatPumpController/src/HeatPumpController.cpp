#include "HeatPumpController.h"

HeatPumpController::HeatPumpController() {
    pinMode(pumpPin, OUTPUT);
}

void HeatPumpController::on() {
    digitalWrite(pumpPin, HIGH);
}

void HeatPumpController::off() {
    digitalWrite(pumpPin, LOW);    
}
