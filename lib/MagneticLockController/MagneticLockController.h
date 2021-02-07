#ifndef __MagneticLockController_H
#define __MagneticLockController_H

#include <Arduino.h>

#define LOCK_BOTTOM_PIN D6
#define LOCK_ROOF_PIN D5

struct LockData
{
  bool locked;
  String status;
};

class MagneticLockController
{
public:
  LockData sensorRoof;
  LockData sensorBottom;

  void setData();
  bool isOk();
  String getAlarmMessage();

private:
  void setSensorData(LockData *data, int);
};

#endif