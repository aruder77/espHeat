#include "ValveController.h"

ValveController::ValveController() {
    setup();
}

void ValveController::setup() {
    pinMode(openPin, OUTPUT);
    pinMode(closePin, OUTPUT);

    digitalWrite(openPin, LOW);
    digitalWrite(closePin, HIGH);

    delay(VALVE_ONE_PERCENT_OPEN_CYCLES * 10 * 100);
    
    digitalWrite(openPin, LOW);
    digitalWrite(closePin, LOW);
}

void ValveController::every10Milliseconds() {
    // adjust 2-point regulation
    if (motorAdjustCounter > 0) {
        if (valveState != 1) {
            Log.trace("opening valve...\n");
            valveState = 1;
            digitalWrite(openPin, HIGH);
            digitalWrite(closePin, LOW);
        }
        if ((motorAdjustCounter % VALVE_ONE_PERCENT_OPEN_CYCLES) == 0 && valveCurrent < 100) {
            valveCurrent++;
            adjustTargetValvePosition();
        }
        motorAdjustCounter--;    
    } else if (motorAdjustCounter < 0) {
        if (valveState != 1) {
            Log.trace("closing valve...\n");
            valveState = -1;
            digitalWrite(openPin, LOW);
            digitalWrite(closePin, HIGH);
        }
        if ((motorAdjustCounter % VALVE_ONE_PERCENT_OPEN_CYCLES) == 0 && valveCurrent > 0) {
            valveCurrent--;
            adjustTargetValvePosition();
        }
        motorAdjustCounter++;
    } else {
        if (valveState != 0) {
            Log.trace("keeping valve state...\n");
            valveState = 0;

            // keep current valve position
            digitalWrite(openPin, LOW);
            digitalWrite(closePin, LOW);
        }
        adjustTargetValvePosition();
    }
}

void ValveController::adjustTargetValvePosition() {
    // check, if new target was set in the meantime...
    if (tempValveTarget != valveTarget) {
        this->valveTarget = tempValveTarget;
        Log.verbose("Ventil Ziel: %d\n", valveTarget);

        // if completely open or closed, make sure it is really completely open/closed.
        if (valveTarget == 100 && valveCurrent < 100) {
            valveTarget = 103;
        } else if (valveTarget == 0 && valveCurrent > 0) {
            valveTarget = -3;
        }
        motorAdjustCounter = max(-103.0, min(103.0, (double)(valveTarget - valveCurrent))) * VALVE_ONE_PERCENT_OPEN_CYCLES;
        Log.trace("MotorAdjustCounter: %d\n", motorAdjustCounter);
    }
}

void ValveController::setTargetValvePosition(int valveTarget) {
    tempValveTarget = valveTarget;
}


void ValveController::configUpdate(const char *id, const char *value) {
    Log.notice("ValveController config update: %s\n", id);
}

int ValveController::getValveCurrent() {
    return valveCurrent;
}

int ValveController::getValveTarget() {
    return valveTarget;
}