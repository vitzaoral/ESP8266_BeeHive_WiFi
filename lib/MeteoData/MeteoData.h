#ifndef __MeteoData_H
#define __MeteoData_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>

struct TempAndHumidity
{
    float temperature;
    float humidity;
};

class MeteoData
{
  public:
    TempAndHumidity sensor;
    void initializeSensor();
    void setData();

  private:
    void setSensorData(TempAndHumidity *data);
    void printSensorData(TempAndHumidity *data);
};

#endif