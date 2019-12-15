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

void HeatPumpController::configUpdate(const char *id, const char *value) {
    Log.notice("HeatPumpController config update: %s\n", id);
}