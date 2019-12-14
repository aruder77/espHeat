#include "HeatingController.h"

HeatingController::HeatingController() {
    prefs->registerConfigParam("kp", "PID-Konstante Kp", "2", 5, this);
    prefs->registerConfigParam("tn", "PID-Konstante Tn", "120", 5, this); 
    prefs->registerConfigParam("flowTemperatureSlope", "Vorlauftemperatur Steigung", "-0.5", 5, this); 
    prefs->registerConfigParam("flowTemperatureOrigin", "Vorlauftemperatur Ursprung (0°)", "30", 5, this); 
}

const char *HeatingController::getName() {
    return "heating";
}

void HeatingController::setup() { 
    temperatureReader = new TemperatureReader();
    flowTemperatureRegulator = new FlowTemperatureRegulator();
    targetFlowTemperatureCalculator = new TargetFlowTemperatureCalculator();
    valveController = new ValveController();
    heatPumpController = new HeatPumpController();

    kp = prefs->getDouble("kp");
    tn = prefs->getDouble("tn");
    flowTemperatureRegulator->setTunings(kp, tn);

    flowTempSlope = prefs->getDouble("flowTemperatureSlope");
    flowTempOrigin = prefs->getDouble("flowTemperatureOrigin");
    targetFlowTemperatureCalculator->setFlowTemperatureSlope(flowTempSlope);
    targetFlowTemperatureCalculator->setFlowTemperatureSlope(flowTempOrigin);

    // always on for now
    heatPumpController->on();
}

void HeatingController::configUpdate(const char *id, const char *value) {
    Log.notice("heatingController config update: %s\n", id);
    if (strcmp(id, "kp") == 0) {
        flowTemperatureRegulator->setTunings(atof(value), tn);
    } else if (strcmp(id, "tn") == 0) {
        flowTemperatureRegulator->setTunings(kp, atof(value));
    } else if (strcmp(id, "flowTemperatureSlope") == 0) {
        flowTempSlope = atof(value);
        targetFlowTemperatureCalculator->setFlowTemperatureSlope(flowTempSlope);
    } else if (strcmp(id, "flowTemperatureOrigin") == 0) {
        flowTempOrigin = atof(value);
        targetFlowTemperatureCalculator->setFlowTemperatureSlope(flowTempOrigin);
    }
}


void HeatingController::everySecond() {
    // every minute
    if (loopCounter == 10) {
        double flowTemperature = temperatureReader->getFlowTemperature();
        double outsideTemperature = temperatureReader->getOutsideTemperature();

        double targetFlowTemperature = targetFlowTemperatureCalculator->calculateTargetFlowTemperature(outsideTemperature);
        double valveTarget = flowTemperatureRegulator->calculateValveTarget(flowTemperature, targetFlowTemperature);
        char printStr[100];
        sprintf(printStr, "Ventil aktuell: %.1f, Ziel: %.1f\n", valveCurrent, valveTarget);
        Log.notice(printStr);
        valveController->setTargetValvePosition(valveTarget);
        valveCurrent = valveTarget;

        loopCounter = 0;
    }
    loopCounter++;
}

void HeatingController::every10Milliseconds() {
    valveController->every10Milliseconds();
    temperatureReader->readTemperatures();
}

void HeatingController::getTelemetryData(char *targetBuffer) {
    double outsideTemperature = temperatureReader->getOutsideTemperature();
    double flowTemperature = temperatureReader->getFlowTemperature();
    double returnTemperature = temperatureReader->getReturnTemperature();
    int valveCurrent = valveController->getValveCurrent();
    int valveTarget = valveController->getValveTarget();

    char telemetryData[100] = {0};
    sprintf(telemetryData, "{\"outside\":%.1f,\"return\":%.1f,\"flow\":%.1f,\"valveCurrent\":%d,\"valveTarget\":%d}", outsideTemperature, returnTemperature, flowTemperature, valveCurrent, valveTarget);
    strcpy(targetBuffer, telemetryData);
}