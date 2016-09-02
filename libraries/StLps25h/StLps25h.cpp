#include "Energia.h"
#include "StLps25h.h"

#define DEFAULT_DEV_ADDR                                 0x5C

/** ID to identify the device. */
#define ST_LPS25H_ID                                     (0xBD)

extern void I2CWriteRead(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                        void *pRdBuff, uint16_t rdBuffLen);

extern void I2CWriteWrite(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                        void *pWrBuff, uint16_t wrBuffLen);

/** Enumerates the registers available on this device. */
typedef enum
{
   /** Queries a value identifying the device. */
   DEV_REG_WHO_AM_I                                      = 0x0F,

   /** Configures temperature and pressure resolution. */
   DEV_REG_RES_CONF                                      = 0x10,

   /** Control register 1. */
   DEV_REG_CTRL_REG1                                     = 0x20,

   /** Control register 2. */
   DEV_REG_CTRL_REG2                                     = 0x21,

   /** Control register 3. */
   DEV_REG_CTRL_REG3                                     = 0x22,

   /** Control register 4. */
   DEV_REG_CTRL_REG4                                     = 0x23,

   /** Interrupt configuration register. */
   DEV_REG_INT_CFG                                       = 0x24,

   /** Status register. */
   DEV_REG_STATUS                                        = 0x27,

   /** Least-significant byte of the measured pressure. */
   DEV_REG_PRESS_OUT_XL                                  = 0x28,

   /** Middle byte of the measured pressure. */
   DEV_REG_PRESS_OUT_L                                   = 0x29,

   /** Most-significant byte of the measured pressure. */
   DEV_REG_PRESS_OUT_H                                   = 0x2A,

   /** Least-significant byte of the measured temperature. */
   DEV_REG_TEMP_OUT_L                                    = 0x2B,

   /** Most-significant byte of the measured temperature. */
   DEV_REG_TEMP_OUT_H                                    = 0x2C,

   /** If this is OR'ed with the register, then it is auto-incremented. */
   DEV_REG_AUTO_INC                                      = 0x80,

} DEV_REG;

StLps25h::StLps25h(uint8_t devAddr, Stream *pLog)
{
   this->devAddr = devAddr;
   this->pLog = pLog;
}

void StLps25h::RegWrite(uint8_t reg, uint8_t value)
{
  I2CWriteWrite(devAddr, &reg, 1, &value, 1);
}

void StLps25h::RegRead(uint8_t reg, uint8_t *pValue)
{
   I2CWriteRead(devAddr, &reg, 1, pValue, 1);
}

void StLps25h::begin(void)
{
   uint8_t data;

   pLog->println("Initializing ST LPS25H...");

   pLog->println("Reading device ID register...");
   RegRead(DEV_REG_WHO_AM_I, &data);
   if (data != ST_LPS25H_ID)
   {
      pLog->println("Invalid device ID.");
      return;
   }

   pLog->println("Configuring device...");
   RegWrite(DEV_REG_CTRL_REG1, (1 << 7) | (1 << 2));
   RegWrite(DEV_REG_CTRL_REG2, 0);
   RegWrite(DEV_REG_CTRL_REG3, 0);
   RegWrite(DEV_REG_CTRL_REG4, 0);
   RegWrite(DEV_REG_INT_CFG, 0);
   RegWrite(DEV_REG_RES_CONF, 0x0F);

   pLog->println("Device initialized and operational.");
}

bool StLps25h::DataReady(void)
{
  uint8_t resp;

  // Query the status register.
  RegRead(DEV_REG_STATUS, &resp);

  return (resp & 0x3) == 3;
}

void StLps25h::readVals(float &temperature, float &pressure)
{
   uint8_t rawData[3];
   int32_t rawPres;
   int16_t rawTemp;
   uint8_t cmd;

   // Trigger a new measurement of both temperature and pressure.
   RegWrite(DEV_REG_CTRL_REG2, (1 << 0));

   // Wait for the measurement to be complete.
   while (!DataReady());

   // Read the raw temperature.
   cmd = DEV_REG_TEMP_OUT_L | DEV_REG_AUTO_INC;
   I2CWriteRead(devAddr, &cmd, 1, &rawTemp, 2);

   // Convert the temperature.
   temperature = rawTemp / 480.0 + 42.5;

   // Read the raw pressure.
   cmd = DEV_REG_PRESS_OUT_XL | DEV_REG_AUTO_INC;
   I2CWriteRead(devAddr, &cmd, 1, &rawData, 3);

   /* The raw data is in 24 bits in LSB. Convert to a full-range S32
   to preserve the sign bit. */
   rawPres = (rawData[0] << 8) | (rawData[1] << 16) | (rawData[2] << 24);

   /* Divide by 256 to get back to 24 bits. Using a right shift is not
   portable and we cannot rely on it to extend the sign bit. */
   rawPres /= 256;

   // Convert the pressure.
   pressure = rawPres / 4096.0;
}

void StLps25h::InvertIrq(bool invert)
{
   if (invert)
   {
      RegWrite(0x22, 0x80);
   }
   else
   {
      RegWrite(0x22, 0x00);
   }
}
