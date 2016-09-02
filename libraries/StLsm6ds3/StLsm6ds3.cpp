#include "Energia.h"
#include "StLsm6ds3.h"

#define DEFAULT_DEV_ADDR               0x6A

extern void I2CWriteRead(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                        void *pRdBuff, uint16_t rdBuffLen);

extern void I2CWriteWrite(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                        void *pWrBuff, uint16_t wrBuffLen);

typedef enum
{
   DEV_REG_INT1_CTRL                                     = 0x0D,
   DEV_REG_INT2_CTRL                                     = 0x0E,
   DEV_REG_WHO_AM_I                                      = 0x0F,
   DEV_REG_CTRL1_XL                                      = 0x10,
   DEV_REG_CTRL2_G                                       = 0x11,
   DEV_REG_CTRL3_C                                       = 0x12,
   DEV_REG_CTRL4_C                                       = 0x13,
   DEV_REG_CTRL5_C                                       = 0x14,
   DEV_REG_CTRL6_C                                       = 0x15,
   DEV_REG_CTRL7_G                                       = 0x16,
   DEV_REG_CTRL8_XL                                      = 0x17,
   DEV_REG_CTRL9_XL                                      = 0x18,
   DEV_REG_CTRL10_C                                      = 0x19,
   DEV_REG_STATUS_REG                                    = 0x1E,
   DEV_REG_OUT_TEMP_L                                    = 0x20,
   DEV_REG_OUT_TEMP                                      = 0x21,
   DEV_REG_OUTX_L_G                                      = 0x22,
   DEV_REG_OUTX_H_G                                      = 0x23,
   DEV_REG_OUTY_L_G                                      = 0x24,
   DEV_REG_OUTY_H_G                                      = 0x25,
   DEV_REG_OUTZ_L_G                                      = 0x26,
   DEV_REG_OUTZ_H_G                                      = 0x27,
   DEV_REG_OUTX_L_XL                                     = 0x28,
   DEV_REG_OUTX_H_XL                                     = 0x29,
   DEV_REG_OUTY_L_XL                                     = 0x2A,
   DEV_REG_OUTY_H_XL                                     = 0x2B,
   DEV_REG_OUTZ_L_XL                                     = 0x2C,
   DEV_REG_OUTZ_H_XL                                     = 0x2D,

} DEV_REG;

StLsm6ds3::StLsm6ds3(uint8_t devAddr, Stream *pLog)
{
   this->devAddr = devAddr;
   this->pLog = pLog;
}

void StLsm6ds3::begin(void)
{
   uint8_t data;

   pLog->println("Initializing ST LSM6DS3...");

   pLog->println("Reading device ID register...");
   RegRead(DEV_REG_WHO_AM_I, &data);
   if (data != 0x69)
   {
      pLog->println("Invalid device ID.");
      return;
   }

   pLog->println("Resetting device...");
   RegWrite(DEV_REG_CTRL3_C, 0x01);
   delay(50);

   pLog->println("Configuring device...");
   RegWrite(DEV_REG_CTRL1_XL, 0x60);
   RegWrite(DEV_REG_CTRL2_G, 0x60);
   RegWrite(DEV_REG_CTRL3_C, (1 << 2) | (1 << 6));

   pLog->println("Device initialized and operational.");
}

void StLsm6ds3::readTemperature(float &temperature)
{
   int16_t rawTemp;
   uint8_t cmd;

   while (!DataReady());

   // Read the raw temperature.
   cmd = DEV_REG_OUT_TEMP_L;
   I2CWriteRead(devAddr, &cmd, 1, &rawTemp, 2);

    // Convert the temperature.
    temperature = rawTemp / 16.0 + 25.0;
}

void StLsm6ds3::readAccel(float accel[3])
{
   int16_t rawAccel[3];
   uint8_t cmd;

   // Read the raw accelerometer.
   cmd = DEV_REG_OUTX_L_XL;
   I2CWriteRead(devAddr, &cmd, 1, &rawAccel, 6);

   // Convert the accelerometer data.
   for (int i = 0; i < 3; i++)
   {
      accel[i] = rawAccel[i] / 16384.0;
   }
}

void StLsm6ds3::readGyro(float gyro[3])
{
   int16_t rawGyro[3];
   uint8_t cmd;

   // Read the raw gyroscope.
   cmd = DEV_REG_OUTX_L_G;
   I2CWriteRead(devAddr, &cmd, 1, &rawGyro, 6);

   // Convert the gyro data.
   for (int i = 0; i < 3; i++)
   {
      gyro[i] = rawGyro[i] * 7.477 / 1000.0;
   }
}

void StLsm6ds3::RegWrite(uint8_t reg, uint8_t value)
{
  I2CWriteWrite(devAddr, &reg, 1, &value, 1);
}

void StLsm6ds3::RegRead(uint8_t reg, uint8_t *pValue)
{
   I2CWriteRead(devAddr, &reg, 1, pValue, 1);
}

bool StLsm6ds3::DataReady(void)
{
  uint8_t resp;

  // Query the status register.
  RegRead(DEV_REG_STATUS_REG, &resp);

  // Check the status depending on which logical sensor is being read. */
  return (resp & 0x7) == 7;
}

void StLsm6ds3::InvertIrq(bool invert)
{
   if (invert)
   {
      RegWrite(0x12, 0x24);
   }
   else
   {
      RegWrite(0x12, 0x04);
   }
}
