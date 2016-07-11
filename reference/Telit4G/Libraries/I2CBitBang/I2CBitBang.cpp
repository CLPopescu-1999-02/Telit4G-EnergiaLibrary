#include "Energia.h"
#include "I2CBitBang.h"

/*******************************************************************************
 Defines
*******************************************************************************/

/** Macro to delay for a half-clock cycle. */
#define DelayHalfClock()   {DelayQuarterClock(); DelayQuarterClock();}

#define u8                 uint8_t

/*******************************************************************************
 Typedefs and Enums
*******************************************************************************/

/** The possible states of the I2C GPIO lines. */
typedef enum
{
   GPIOSTATE_LOW   = 0,
   GPIOSTATE_HIGH  = 1,
} I2cGpioState_e;

/** Whether a byte was ACKed or NACKed. */
typedef enum
{
   ACK   = 0,
   NACK  = 1,
} I2cAckState_e;

/** The values of the I2C read/write bit. */
typedef enum
{
   I2C_WRITE = 0,
   I2C_READ  = 1,
} I2cRw_e;

/*******************************************************************************
 Module Variables.
*******************************************************************************/

/** The last value the I2C clock line was set to. */
static u8 curClkVal = 1;

/** The last value the I2C data line was set to. */
static u8 curDataVal = 1;

static int scl = 9, sda = 10;

/**************************************************************************//**
* Delays for one-quarter of a clock cycle.
*
* @returns OK on success, or a negative error value on error.
******************************************************************************/
static void DelayQuarterClock(void)
{
   delayMicroseconds(3);
}

/**************************************************************************//**
* Reads the current value on the clock line.
*
* @returns 1 if the clock line is GPIOSTATE_HIGH, 0 if not.
******************************************************************************/
static u8 I2cClockRead(void)
{
   return digitalRead(scl);
}

/**************************************************************************//**
* Reads the current value on the data line.
*
* @returns 1 if the data line is GPIOSTATE_HIGH, 0 if not.
******************************************************************************/
static u8 I2cDataRead(void)
{
   return digitalRead(sda);
}

/**************************************************************************//**
* Sets the clock line high or low.
*
* @returns OK on success, or a negative error value on error.
******************************************************************************/
static void I2cClockSet(I2cGpioState_e state)
{
   if (curClkVal != state)
   {
      curClkVal = state;

      if (state == GPIOSTATE_LOW)
      {
         // Change the pin to an output. It will begin driving low.
         pinMode(scl, OUTPUT);
      }
      else
      {
         // Change the pin to an input. It will be pulled up high.
         pinMode(scl, INPUT);

         // Implement clock-stretching by polling until the line is high.
         while (I2cClockRead() == GPIOSTATE_LOW);
      }
   }
}

/**************************************************************************//**
* Sets the data line high or low.
*
* @returns OK on success, or a negative error value on error.
******************************************************************************/
static void I2cDataSet(I2cGpioState_e state)
{
   if (curDataVal != state)
   {
      curDataVal = state;
      if (state == GPIOSTATE_LOW)
      {
         // Change the pin to an output. It will begin driving low.
         pinMode(sda, OUTPUT);
      }
      else
      {
         // Change the pin to an input. It will be pulled up high.
         pinMode(sda, INPUT);
      }
   }
}

/**************************************************************************//**
* Generates a start condition on the I2C bus.
*
* The clock and data lines should be in the GPIOSTATE_HIGH state before calling this function.
*
* @returns None.
******************************************************************************/
static void I2cBitBang_GenerateStartCondition(void)
{
   I2cDataSet(GPIOSTATE_LOW);
   DelayHalfClock();

   I2cClockSet(GPIOSTATE_LOW);
   DelayHalfClock();
}

/**************************************************************************//**
* Generates a stop condition on the I2C bus.
*
* The clock should be in the GPIOSTATE_LOW state before calling this function.
*
* @returns None.
******************************************************************************/
static void I2cBitBang_GenerateStopCondition(void)
{
   I2cDataSet(GPIOSTATE_LOW);
   DelayHalfClock();

   I2cClockSet(GPIOSTATE_HIGH);
   DelayHalfClock();

   I2cDataSet(GPIOSTATE_HIGH);
   DelayHalfClock();
}

/**************************************************************************//**
* Generates and ACK bit cycle and queries if the device ACKed or NACKed.
*
* The clock line should be in the GPIOSTATE_LOW state before calling this function.
*
* @returns Whether or not the slave device ACKed or NACKed the byte.
******************************************************************************/
static I2cAckState_e I2cBitBang_CheckAck(void)
{
   I2cAckState_e ackState;

   // Release the data line so the slave can drive it.
   I2cDataSet(GPIOSTATE_HIGH);

   I2cClockSet(GPIOSTATE_HIGH);
   DelayHalfClock();

   // Sample the data line to see if the slave is ACKing or NACKing
   ackState = (I2cDataRead() == GPIOSTATE_HIGH) ? NACK : ACK;

   I2cClockSet(GPIOSTATE_LOW);
   DelayHalfClock();

   return ackState;
}

/**************************************************************************//**
* Generates either an ACK or a NACK on the I2C bus.
*
* The clock line should be in the GPIOSTATE_LOW state before calling this function.
*
* @returns None.
******************************************************************************/
static void I2cBitBang_GenerateAckBit(I2cAckState_e ackState)
{
   I2cDataSet(ackState == ACK ? GPIOSTATE_LOW : GPIOSTATE_HIGH);
   DelayHalfClock();

   I2cClockSet(GPIOSTATE_HIGH);
   DelayHalfClock();

   I2cClockSet(GPIOSTATE_LOW);
   DelayHalfClock();
}

/**************************************************************************//**
* Clocks the given 8-bit byte out on the I2C bus.
*
* The clock line should be in the GPIOSTATE_LOW state before calling this function.
*
* @returns None.
******************************************************************************/
static void I2cBitBang_WriteByte(u8 data)
{
   u8 i;

   for (i=0;i<8;i++)
   {
      I2cDataSet((data & 0x80) != 0 ? GPIOSTATE_HIGH : GPIOSTATE_LOW);
      data <<= 1;
      DelayQuarterClock();

      I2cClockSet(GPIOSTATE_HIGH);
      DelayHalfClock();

      I2cClockSet(GPIOSTATE_LOW);
      DelayHalfClock();
   }
}

/**************************************************************************//**
* Clocks an 8-bit byte and reads it from the I2C bus.
*
* The clock line should be in the GPIOSTATE_LOW state before calling this function.
*
* @returns The 8-bit data read from the I2C bus.
******************************************************************************/
static u8 I2cBitBang_ReadByte(void)
{
   u8 i, data;

   // Release the data line so the slave can drive it.
   I2cDataSet(GPIOSTATE_HIGH);

   data = 0;
   for (i=0;i<8;i++)
   {
      I2cClockSet(GPIOSTATE_HIGH);
      DelayHalfClock();

      data <<= 1;
      data |= (I2cDataRead() & 0x01);

      I2cClockSet(GPIOSTATE_LOW);
      DelayHalfClock();
   }

   return data;
}

/*******************************************************************************
 Public Functions.
*******************************************************************************/

void I2CBitBang_Init(int pinScl, int pinSda)
{
   scl = pinScl;
   sda = pinSda;

   // Set the bus lines as inputs, allowing them to float high
   pinMode(scl, INPUT);
   pinMode(sda, INPUT);

   // Write LOW to disable the pullups on these pins, and to pre-set the output
   // value for when we change its mode to an output.
   digitalWrite(scl, LOW);
   digitalWrite(sda, LOW);
}

void I2CWriteRead(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                  void *pRdBuff, uint16_t rdBuffLen)
{
   I2cAckState_e ackState = ACK;
   u8 *pCmdU8 = (u8 *) pCmd;
   u8 *pRdBuffU8 = (u8 *) pRdBuff;

   /* Generate a start condition on the I2C bus. */
   I2cBitBang_GenerateStartCondition();

   /* See if we need to write some data before issuing the read command. */
   if (cmdLen > 0)
   {
      /* Write the slave address plus the write bit. */
      I2cBitBang_WriteByte((devAddr << 1) | I2C_WRITE);

      /* See if the slave ACKed the slave address byte. */
      ackState = I2cBitBang_CheckAck();

      /* Loop through the data to write to the slave before issuing the read. */
      while ((ackState == ACK) && (cmdLen > 0))
      {
         I2cBitBang_WriteByte(*pCmdU8);
         ackState = I2cBitBang_CheckAck();
         cmdLen--;
         pCmdU8++;
      }

      if (ackState == ACK)
      {
         /* At this point, the clock is low and the data line is high from I2cBitBang_CheckAck(). */
         I2cClockSet(GPIOSTATE_HIGH);
         DelayHalfClock();

         /* Now generate a repeated start condition. */
         I2cBitBang_GenerateStartCondition();
      }
   }

   if (ackState == ACK)
   {
      /* Write the slave address plus the read bit. */
      I2cBitBang_WriteByte((devAddr << 1) | I2C_READ);

      /* See if the slave ACKed the slave address byte. */
      ackState = I2cBitBang_CheckAck();

      /* Read each data byte from the slave. The master generates the ACKs in this case. */
      while ((ackState == ACK) && (rdBuffLen > 0))
      {
         *pRdBuffU8 = I2cBitBang_ReadByte();
         I2cBitBang_GenerateAckBit((rdBuffLen > 1) ? ACK : NACK);
         rdBuffLen--;
         pRdBuffU8++;
      }
   }

   /* Generate the stop condition on the I2C bus. */
   I2cBitBang_GenerateStopCondition();
}

void I2CWriteWrite(uint8_t devAddr, const void *pCmd, uint16_t cmdLen,
                   void *pWrBuff, uint16_t wrBuffLen)
{
   I2cAckState_e ackState;
   u8 *pBuff1 = (u8 *) pCmd;
   u8 *pBuff2 = (u8 *) pWrBuff;

   /* Generate a start condition on the I2C bus. */
   I2cBitBang_GenerateStartCondition();

   /* Write the slave address plus the write bit. */
   I2cBitBang_WriteByte((devAddr << 1) | I2C_WRITE);

   /* See if the slave ACKed the slave address byte. */
   ackState = I2cBitBang_CheckAck();

   /* Loop through each data byte to write from buffer 1 so long as the slave ACKs the data. */
   while ((ackState == ACK) && (cmdLen > 0))
   {
      I2cBitBang_WriteByte(*pBuff1);
      ackState = I2cBitBang_CheckAck();
      cmdLen--;
      pBuff1++;
   }

   /* Loop through each data byte to write from buffer 2 so long as the slave ACKs the data. */
   while ((ackState == ACK) && (wrBuffLen > 0))
   {
      I2cBitBang_WriteByte(*pBuff2);
      ackState = I2cBitBang_CheckAck();
      wrBuffLen--;
      pBuff2++;
   }

   /* Generate the stop condition on the I2C bus. */
   I2cBitBang_GenerateStopCondition();
}
