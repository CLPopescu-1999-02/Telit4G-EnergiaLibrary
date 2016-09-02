#ifndef TI_HDC1050_H
#define TI_HDC1050_H

#include "Energia.h"

class TiHdc1050
{
public:
   TiHdc1050(Stream *pLog);
   void begin(void);

private:
   Stream *pLog;
   void RegWrite(uint8_t reg, uint16_t value);
   void RegRead(uint8_t reg, uint16_t *pValue);
};

#endif
