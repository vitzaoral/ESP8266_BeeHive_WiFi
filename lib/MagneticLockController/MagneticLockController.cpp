#include "MagneticLockController.h"

void MagneticLockController::setData()
{
    // clear "wires memory"
    // on long cables it works slowly, so set tu LOW and check if is HIGH
    pinMode(LOCK_ROOF_PIN, OUTPUT);
    digitalWrite(LOCK_ROOF_PIN, LOW);

    delay(200);
    pinMode(LOCK_ROOF_PIN, INPUT);

    Serial.print("Magnetic lock roof: ");
    setSensorData(&sensorRoof, LOCK_ROOF_PIN);
}

void MagneticLockController::setSensorData(LockData *data, int pin)
{
    if (digitalRead(pin) == HIGH)
    {
        data->locked = true;
        data->status = "OK";
        Serial.println("OK");
    }
    else
    {
        data->locked = false;
        data->status = "ALARM!";
        Serial.println("UNLOCKED!");
    }
}

// Check if all lockers are locked
bool MagneticLockController::isOk()
{
    return sensorRoof.locked;
}

// alarm message for alarm notification
String MagneticLockController::getAlarmMessage()
{
    String message = "";
    return String(sensorRoof.locked ? "" : "roof");
}