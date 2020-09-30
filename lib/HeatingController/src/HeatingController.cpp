#include "HeatingController.h"

HeatingController::HeatingController() {
    prefs->registerConfigParam("kp", "PID-Konstante Kp", "2", 5, this);
    prefs->registerConfigParam("tn", "PID-Konstante Tn", "120", 5, this); 
    prefs->registerConfigParam("flowTemperatureSlope", "Vorlauftemperatur Steigung", "-0.5", 5, this); 
    prefs->registerConfigParam("flowTemperatureOrigin", "Vorlauftemperatur Ursprung (0°)", "32", 5, this); 
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
    targetFlowTemperatureCalculator->setFlowTemperatureOrigin(flowTempOrigin);

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
        targetFlowTemperatureCalculator->setFlowTemperatureOrigin(flowTempOrigin);
    }
}


void HeatingController::everySecond() {
    // every 10 seconds
    if (loopCounter == 10) {
        double flowTemperature = temperatureReader->getFlowTemperature();
        double outsideTemperature = temperatureReader->getOutsideTemperature();

        double targetFlowTemperature = targetFlowTemperatureCalculator->calculateTargetFlowTemperature(outsideTemperature);
        int valveTarget = flowTemperatureRegulator->calculateValveTarget(flowTemperature, targetFlowTemperature);
        valveController->setTargetValvePosition(valveTarget);
        char printStr[100];
        sprintf(printStr, "Ventil aktuell: %d, Ziel: %d\n", valveController->getValveCurrent(), valveTarget);
        Log.notice(printStr);

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
    double targetFlowTemperature = targetFlowTemperatureCalculator->calculateTargetFlowTemperature(outsideTemperature);


    char telemetryData[200] = {0};
    sprintf(telemetryData, "{\"outside\":%.1f,\"return\":%.1f,\"flow\":%.1f,\"targetFlowTemperature\":%.1f,\"valveCurrent\":%d,\"valveTarget\":%d}", outsideTemperature, returnTemperature, flowTemperature, targetFlowTemperature, valveCurrent, valveTarget);
    strcpy(targetBuffer, telemetryData);
}