#ifndef __MagneticLockController_H
#define __MagneticLockController_H

#include <Arduino.h>

#define LOCK_ROOF_PIN D3

struct LockData
{
  bool locked;
  String status;
};

class MagneticLockController
{
public:
  LockData sensorRoof;

  void setData();
  bool isOk();
  String getAlarmMessage();

private:
  void setSensorData(LockData *data, int);
};

#endif