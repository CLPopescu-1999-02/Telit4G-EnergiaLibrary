#include <StLps25h.h>
#include <I2CBitBang.h>

// Tests the ST LPS25H temperature and pressure sensor on the MSP430 F5529 LaunchPad
// using the bit-banging I2C library. The Wire I2C library for this LaunchPad defaults
// to the wrong I2C port, and there is no way to change it. The bit-banging I2C library
// bit-bangs the correct pins, 9 and 10.

StLps25h myStLps25h(0x5C, &Serial);

void setup()
{
  Serial.begin(115200);
  I2CBitBang_Init(9, 10);
  myStLps25h.begin();
}

void loop()
{
  uint8_t cmd, data;
  
  Serial.println("Testing ST-LPS25H temperature/humidity sensor...");

  while (true)
  {
    float temp, pres;
    
    // Read data from the device.
    myStLps25h.readVals(temp, pres);
    
    Serial.print("T=");
    Serial.print(temp);
    Serial.print(", P=");
    Serial.println(pres);
  }
}

