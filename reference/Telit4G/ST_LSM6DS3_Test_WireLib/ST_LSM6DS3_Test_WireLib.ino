#include <energia.h>
#include <StLsm6ds3.h>
#include <I2CWireLib.h>
#include <Wire.h>

// Tests the ST LSM6DS3 accelerometer using the Wire I2C library. Works on
// the Tiva C LaunchPad with the default I2C pins. For the MSP430 F5529
// LaunchPad, the Wire I2C library defaults to the wrong I2C port, and there
// is no way to change it. Therefore, the accelerometer must be wired up to
// pins 14 and 15, instead of 9 and 10.

StLsm6ds3 myStLsm6ds3(0x6A, &Serial);

void setup()
{
  Serial.begin(115200);
  
#ifdef __TM4C1294NCPDT__

/* Tiva C TM4C1294 LaunchPad. Select the correct I2C port. */
  Wire.setModule(0);

#endif

  Wire.begin();
  myStLsm6ds3.begin();
}

void loop()
{
  uint8_t cmd, data;
  
  Serial.println("Testing ST-LSM6DS3 accelerometer...");

  while (true)
  {
    float temp, accel[3], gyro[3];
    
    // Read data from the device.
    myStLsm6ds3.readTemperature(temp);
    myStLsm6ds3.readAccel(accel);
    myStLsm6ds3.readGyro(gyro);
    
    Serial.print("T=");
    Serial.print(temp);
    Serial.print(", A=<");
    Serial.print(accel[0]);
    Serial.print(",");
    Serial.print(accel[1]);
    Serial.print(",");
    Serial.print(accel[2]);
    Serial.print(">, G=<");
    Serial.print(gyro[0]);
    Serial.print(",");
    Serial.print(gyro[1]);
    Serial.print(",");
    Serial.print(gyro[2]);
    Serial.println(">");
  }
}

