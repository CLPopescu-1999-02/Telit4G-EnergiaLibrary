#ifndef ST_LSM6DS3_H
#define ST_LSM6DS3_H

#include "Energia.h"

class StLsm6ds3
{
public:
   StLsm6ds3(uint8_t devAddr, Stream *pLog);
   void begin(void);
   void readTemperature(float &temperature);
   void readAccel(float accel[3]);
   void readGyro(float gyro[3]);
   void InvertIrq(bool invert);

private:
   Stream *pLog;
   uint8_t devAddr;
   void RegWrite(uint8_t reg, uint8_t value);
   void RegRead(uint8_t reg, uint8_t *pValue);
   bool DataReady(void);
};

#endif
