#ifndef ST_LPS25H_H
#define ST_LPS25H_H

#include "Energia.h"

class StLps25h
{
public:
   StLps25h(uint8_t devAddr, Stream *pLog);
   void begin(void);
   void readVals(float &temperature, float &pressure);
   void InvertIrq(bool invert);

private:
   Stream *pLog;
   uint8_t devAddr;
   void RegWrite(uint8_t reg, uint8_t value);
   void RegRead(uint8_t reg, uint8_t *pValue);
   bool DataReady(void);
};

#endif
