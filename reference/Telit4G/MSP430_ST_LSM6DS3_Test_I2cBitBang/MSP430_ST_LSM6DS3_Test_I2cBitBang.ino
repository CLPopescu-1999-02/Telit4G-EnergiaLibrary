#include <I2CBitBang.h>
#include <StLsm6ds3.h>

// Tests the ST LSM6DS3 accelerometer on the MSP430 F5529 LaunchPad using
// the bit-banging I2C library. The Wire I2C library for this LaunchPad
// defaults to the wrong I2C port, and there is no way to change it. The
// bit-banging I2C library bit-bangs the correct pins, 9 and 10.
StLsm6ds3 myStLsm6ds3(0x6A, &Serial);

void setup()
{
  Serial.begin(115200);
  I2CBitBang_Init(9, 10);
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

