#include "Energia.h"
#include "I2CWireLib.h"
#include <Wire.h>

void I2CWriteRead(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                  void *pRdBuff, uint16_t rdBuffLen)
{
  uint8_t *pCmdU8 = (uint8_t *) pCmd;
  uint8_t *pRdBuffU8 = (uint8_t *) pRdBuff;
  int avail;

  // Generate start condition and write the slave address.
  Wire.beginTransmission(devAddr);

  // Queue up data for the initial command.  
  while (cmdLen--)
  {
    Wire.write(*pCmdU8++);
  }

  // Send all queued command bytes to write.
  Wire.endTransmission();

  // Request to read the data from the I2C bus.
  Wire.requestFrom(devAddr, rdBuffLen);

  while (rdBuffLen > 0)
  {
    // Wait until at least one byte is available to read.
    while (!(avail = Wire.available()));
    
    // Read all available data.
    while (avail--)
    {
      *pRdBuffU8++ = Wire.read();
      rdBuffLen--;
    }
  }
}

void I2CWriteWrite(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                   void *pWrBuff, uint16_t wrBuffLen)
{
  uint8_t *pCmdU8 = (uint8_t *) pCmd;
  uint8_t *pWrBuffU8 = (uint8_t *) pWrBuff;

  // Generate start condition and write the slave address.
  Wire.beginTransmission(devAddr);

  // Queue up data for the initial command.  
  while (cmdLen--)
  {
    Wire.write(*pCmdU8++);
  }

  // Queue up data for the write buffer.  
  while (wrBuffLen--)
  {
    Wire.write(*pWrBuffU8++);
  }

  // Send all queued command bytes to write.
  Wire.endTransmission();
}
