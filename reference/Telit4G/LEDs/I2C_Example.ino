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

void RegWrite(uint8_t reg, uint8_t value)
{
  I2CWriteWrite(0x6A, &reg, 1, &value, 1);
}

void RegRead(uint8_t reg, uint8_t *pValue)
{
   I2CWriteRead(0x6A, &reg, 1, pValue, 1);
}

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
}

void loop()
{
  uint8_t cmd, data;
  
  Serial.println("Example I2C application.");

  cmd = 0x0F;
  I2CWriteRead(0x6A, &cmd, 1, &data, 1);
  
  Serial.print("Read data:");
  Serial.println(data);  

  delay(500);
}

