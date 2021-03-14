#include <Arduino.h>
#include <SPI.h>
#include <InternetConnection.h>

InternetConnection connection;
MeteoData meteoData;
MagneticLockController magneticLockController;

void sendDataToInternet();

// DEEPSLEEP: https://www.mischianti.org/2019/11/21/wemos-d1-mini-esp8266-the-three-type-of-sleep-mode-to-manage-energy-savings-part-4/
void setup()
{
  Serial.begin(115200);

  meteoData.initializeSensor();
  magneticLockController.setData();

  Serial.println("Setup done, send  data");
  sendDataToInternet();
  Serial.println("Data sended, BYE BYE");

  ESP.deepSleep(300e6); // 5 minutes
}

void loop()
{
}

void sendDataToInternet()
{
  Serial.println("Setting sensors data");
  meteoData.setData();

  // gyroscope and magnetic locks data are set in other timer more often, so we have actual data
  Serial.println("Start initialize Blynk connection");
  if (connection.initializeConnection())
  {

    Serial.println("Sending data to Blynk");
    connection.sendDataToBlynk(meteoData, magneticLockController);
    connection.checkForUpdates();
    connection.disconnect();
  }
  else
  {
    Serial.println("No internet/blynk connection");
    connection.disconnect();
  }
}