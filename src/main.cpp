#include <Arduino.h>
#include <SPI.h>
#include <InternetConnection.h>
#include <Ticker.h>

InternetConnection connection;
MeteoData meteoData;
MagneticLockController magneticLockController;

void sendDataToInternet();
void checkMagneticLockAlarm();

Ticker timerSendDataToInternet(sendDataToInternet, 273000);  // 4.3 min = 260000ms
Ticker timerMagneticLockAlarm(checkMagneticLockAlarm, 4321); // 4 sec

// alarm section
void sendDataToBlynkIfAlarm();
Ticker timerSendDataToBlynkIfAlarm(sendDataToBlynkIfAlarm, 20000); // 20 sec

void setup() {
  Serial.begin(115200);

  timerSendDataToInternet.start();
  timerMagneticLockAlarm.start();
  timerSendDataToBlynkIfAlarm.start();

  meteoData.initializeSensor();

  // set first data for magnetic locks, other in timers..
  magneticLockController.setData();

  Serial.println("Setup done, send first data");
  sendDataToInternet();
  Serial.println("First data sended, start loop");
}

void loop()
{
  timerSendDataToInternet.update();
  timerMagneticLockAlarm.update();
  timerSendDataToBlynkIfAlarm.update();

  connection.blynkRunIfAlarm();
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

void checkMagneticLockAlarm()
{
  magneticLockController.setData();
  if (magneticLockController.isOk())
  {
    // update blynk data and turn alarm off
    if (connection.isAlarm)
    {
      connection.setMagneticLockControllerDataToBlynkIfAlarm(magneticLockController);
    }
    connection.isAlarm = false;
  }
  else
  {
    connection.isAlarm = true;
    connection.alarmMagneticController(magneticLockController);
  }
}

void sendDataToBlynkIfAlarm()
{
  connection.setMagneticLockControllerDataToBlynkIfAlarm(magneticLockController);
} 