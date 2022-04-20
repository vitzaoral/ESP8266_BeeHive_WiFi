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

  if ((connection.isAlarmEnabled && magneticLockController.isOk()) || !connection.isAlarmEnabled)
  {
    Serial.println("Data sended, BYE BYE");
    ESP.deepSleep(300e6);
    delay(100);
  }
  else if (connection.isAlarmEnabled && !magneticLockController.isOk())
  {
    Serial.println("Data sended, ALARM, BYE BYE for 30 sec");
    ESP.deepSleep(30e6);
    delay(100);
  }
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

    if (magneticLockController.isOk())
    {
      // update blynk data and turn alarm off
      if (connection.isAlarmEnabled)
      {
        connection.setMagneticLockControllerDataToBlynkIfAlarm(magneticLockController);
      }
    }
    else
    {
      connection.alarmMagneticController(magneticLockController);
    }

    connection.disconnect();
  }
  else
  {
    Serial.println("No internet/blynk connection");
    connection.disconnect();
  }
} 