#include "Energia.h"
#include "TiHdc1050.h"

#define I2C_ADDR                                         0x40

/** Manufacturer ID to identify the device as TI ("TI"). */
#define TI_HDC1050_MANUF_ID                              (0x5449)

/** Identifies the device as an HDC 1050. */
#define TI_HDC1050_DEVICE_ID                             (0x1050)

#define OS_SWAP_BYTES_16(a)      ((((uint16_t)(a) & 0x00FF) << 8) | \
                                     (((uint16_t)(a) & 0xFF00) >> 8))

/** Converts the given 16-bit value into a big-endian representation. */
#define OS_BIG_ENDIAN_16(a)      OS_SWAP_BYTES_16(a)

/** A shorter version of OS_BIG_ENDIAN_16. */
#define OS_MSB_16                OS_BIG_ENDIAN_16

extern void I2CWriteRead(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                        void *pRdBuff, uint16_t rdBuffLen);

extern void I2CWriteWrite(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                        void *pWrBuff, uint16_t wrBuffLen);

/** Enumerates the registers available on this device. */
typedef enum
{
   /** Initiates and reads a temperature measurement. */
   DEV_REG_TEMPERATURE                                   = 0x00,

   /** Initiates and reads a humidity measurement. */
   DEV_REG_HUMIDITY                                      = 0x01,

   /** Configures the device. */
   DEV_REG_CONFIG                                        = 0x02,

   /** First 2 bytes of the device's serial ID. */
   DEV_REG_ID_0                                          = 0xFB,

   /** Middle 2 bytes of the device's serial ID. */
   DEV_REG_ID_1                                          = 0xFC,

   /** Last byte of the device's serial ID. */
   DEV_REG_ID_2                                          = 0xFD,

   /** ManufacturerId. Reads 0x5449 ("TI") */
   DEV_REG_MANUF_ID                                      = 0xFE,

   /** Device Id. Identifies this as the HDC1050. Reads 0x1050. */
   DEV_REG_DEV_ID                                        = 0xFF,

} DEV_REG;

TiHdc1050::TiHdc1050(Stream *pLog)
{
   this->pLog = pLog;
}

void TiHdc1050::RegWrite(uint8_t reg, uint16_t value)
{
   value = OS_MSB_16(value);
   I2CWriteWrite(I2C_ADDR, &reg, 1, &value, 2);
}

void TiHdc1050::RegRead(uint8_t reg, uint16_t *pValue)
{
   I2CWriteRead(I2C_ADDR, &reg, 1, pValue, 2);
   *pValue = OS_MSB_16(*pValue);
}

void TiHdc1050::begin(void)
{
   uint16_t resp;

   pLog->println("Initializing TI HDC1050...");

   pLog->println("Reading manufacturer ID register...");
   RegRead(DEV_REG_MANUF_ID, &resp);
   if (resp != TI_HDC1050_MANUF_ID)
   {
      pLog->println("Invalid manufacturer ID.");
      return;
   }

   pLog->println("Reading device ID register...");
   RegRead(DEV_REG_DEV_ID, &resp);
   if (resp != TI_HDC1050_DEVICE_ID)
   {
      pLog->println("Invalid device ID.");
      return;
   }

   pLog->println("Configuring device...");
   RegWrite(DEV_REG_CONFIG, 0);

   pLog->println("Device initialized and operational.");
}
