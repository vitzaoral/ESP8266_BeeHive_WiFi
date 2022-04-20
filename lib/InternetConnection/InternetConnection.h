#ifndef __InternetConnection_H
#define __InternetConnection_H

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <MeteoData.h>
#include <MagneticLockController.h>

class InternetConnection
{
public:
  InternetConnection();
  bool isAlarmEnabled;
  bool initializeConnection();
  void disconnect();
  void sendDataToBlynk(MeteoData, MagneticLockController);
  void alarmMagneticController(MagneticLockController);
  void blynkRunIfAlarm();
  void setMagneticLockControllerDataToBlynkIfAlarm(MagneticLockController);
  void printlnToTerminal(String);
  void checkForUpdates();

private:
  void setMagneticLockControllerDataToBlynk(MagneticLockController);
  void setAlarmInfoToBlynk();
};

#endif 