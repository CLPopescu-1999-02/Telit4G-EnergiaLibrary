#include <TiHdc1050.h>
#include <I2CBitBang.h>

// Tests the TI HDC1050 temperature andhumidity sensor on the MSP430 F5529 LaunchPad
// using the bit-banging I2C library. The Wire I2C library for this LaunchPad defaults
// to the wrong I2C port, and there is no way to change it. The bit-banging I2C library
// bit-bangs the correct pins, 9 and 10.

TiHdc1050 myTiHdc1050(&Serial);

void setup()
{
  Serial.begin(115200);
  I2CBitBang_Init(9, 10);
  myTiHdc1050.begin();
}

void loop()
{
  delay(1000);
  myTiHdc1050.begin();
}

