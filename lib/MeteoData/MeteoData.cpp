#include "MeteoData.h"

// SHT3X sensors
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void MeteoData::initializeSensor()
{
    if (!sht31.begin(0x44))
    {
        Serial.println("Could not find a valid SHT31X sensor on oaddress 0x44!");
    }
    else
    {
        Serial.println("Inner SHT31X OK");
    }
}

void MeteoData::setData()
{
    MeteoData::setSensorData(&sensor);
}

void MeteoData::setSensorData(TempAndHumidity *data)
{
    data->temperature = sht31.readTemperature();
    data->humidity = sht31.readHumidity();
    MeteoData::printSensorData(data);
}

void MeteoData::printSensorData(TempAndHumidity *data)
{
    Serial.print("temperature: " + String(data->temperature) + "°C ");
    Serial.print("humidity: " + String(data->humidity) + "% ");
}