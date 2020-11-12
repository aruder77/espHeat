#include "HeatingController.h"

bool flowTemperatureSlopeHandler(const HomieRange& range, const String& value) {
	float floatValue = value.toFloat();
	if (floatValue < -1 || floatValue > 1) {
		return false;
	}

	HeatingController::getInstance()->setFlowTemperatureSlope(floatValue);
  	return true;
}

bool flowTemperatureOriginHandler(const HomieRange& range, const String& value) {
	float floatValue = value.toFloat();
	if (floatValue < 20 || floatValue > 100) {
		return false;
	}

	HeatingController::getInstance()->setFlowTemperatureOrigin(floatValue);
  	return true;
}

bool pidKpHandler(const HomieRange& range, const String& value) {
	int intValue = value.toInt();
	if (intValue < 0 || intValue > 10) {
		return false;
	}

	HeatingController::getInstance()->setPidKp(intValue);
  	return true;
}

bool pidTnHandler(const HomieRange& range, const String& value) {
	int intValue = value.toInt();
	if (intValue < 0 || intValue > 1000) {
		return false;
	}

	HeatingController::getInstance()->setPidTn(intValue);
  	return true;
}

HeatingController *HeatingController::instance = 0;

HeatingController *HeatingController::getInstance()
{
	// The only instance
	// Guaranteed to be lazy initialized
	// Guaranteed that it will be destroyed correctly
	if (!HeatingController::instance)
	{
		HeatingController::instance = new HeatingController();
	}
	return HeatingController::instance;
}


HeatingController::HeatingController() {
    heatNode = new HomieNode("heating", "heating node", "heating");

	heatNode->advertise("flowTemperatureSlope")
			.setName("flowTemperatureSlope")
			.setDatatype("float")
			.settable(flowTemperatureSlopeHandler);

	heatNode->advertise("flowTemperatureOrigin")
			.setName("flowTemperatureOrigin")
			.setDatatype("float")
			.settable(flowTemperatureOriginHandler);


	heatNode->advertise("pid-kp")
			.setName("PID-Constant Kp")
			.setDatatype("integer")
			.settable(pidKpHandler);

	heatNode->advertise("pid-tn")
			.setName("PID-Constant Tn")
			.setDatatype("integer")
			.settable(pidTnHandler);

    heatNode->advertise("flowTemperature")
            .setName("flow temperature")
            .setDatatype("float")
            .setUnit("°C");

    heatNode->advertise("outsideTemperature")
            .setName("outside temperature")
            .setDatatype("float")
            .setUnit("°C");

    heatNode->advertise("targetFlowTemperature")
            .setName("target flow temperature")
            .setDatatype("float")
            .setUnit("°C");

    heatNode->advertise("valveSetting")
            .setName("current valve setting")
            .setDatatype("integer")
            .setUnit("%");

    heatNode->advertise("valveTarget")
            .setName("valve target setting")
            .setDatatype("integer")
            .setUnit("%");            
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

    flowTemperatureRegulator->setTunings(kp, tn);

    targetFlowTemperatureCalculator->setFlowTemperatureSlope(flowTempSlope);
    targetFlowTemperatureCalculator->setFlowTemperatureOrigin(flowTempOrigin);

    // always on for now
    heatPumpController->on();
}

void HeatingController::afterSetup() {
    heatNode->setProperty("flowTemperatureSlope").send(String(flowTempSlope));
    heatNode->setProperty("flowTemperatureOrigin").send(String(flowTempOrigin));
    heatNode->setProperty("pid-kp").send(String(kp));
    heatNode->setProperty("pid-tn").send(String(tn));
}

void HeatingController::setFlowTemperatureSlope(float slope) {
    this->flowTempSlope = slope;
    targetFlowTemperatureCalculator->setFlowTemperatureSlope(slope);
}

void HeatingController::setFlowTemperatureOrigin(float origin) {
    this->flowTempOrigin = origin;
    targetFlowTemperatureCalculator->setFlowTemperatureOrigin(origin);
}

void HeatingController::setPidKp(int kp) {
    this->kp = kp;
    flowTemperatureRegulator->setTunings(kp, this->tn);
}

void HeatingController::setPidTn(int tn) {
    this->tn = tn;
    flowTemperatureRegulator->setTunings(this->kp, tn);
}


void HeatingController::everySecond() {
    // every 10 seconds
    if (loopCounter == 10) {
        double flowTemperature = temperatureReader->getFlowTemperature();
        double outsideTemperature = temperatureReader->getOutsideTemperature();

        double targetFlowTemperature = targetFlowTemperatureCalculator->calculateTargetFlowTemperature(outsideTemperature);
        int valveTarget = flowTemperatureRegulator->calculateValveTarget(flowTemperature, targetFlowTemperature);
        int valveCurrent = valveController->getValveCurrent();
        valveController->setTargetValvePosition(valveTarget);

        heatNode->setProperty("flowTemperature").send(String(flowTemperature));
        heatNode->setProperty("outsideTemperature").send(String(outsideTemperature));
        heatNode->setProperty("targetFlowTemperature").send(String(targetFlowTemperature));
        heatNode->setProperty("valveTarget").send(String(valveTarget));
        heatNode->setProperty("valveSetting").send(String(valveCurrent));

        char printStr[100];
        sprintf(printStr, "Ventil aktuell: %d, Ziel: %d\n", valveCurrent, valveTarget);
        Homie.getLogger() << printStr;

        loopCounter = 0;
    }
    loopCounter++;
}

void HeatingController::every10Milliseconds() {
    valveController->every10Milliseconds();
    temperatureReader->readTemperatures();
}
