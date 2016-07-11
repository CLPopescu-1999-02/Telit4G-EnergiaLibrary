#include <I2CBitBang.h>
#include <StLsm6ds3.h>

// Tests the ST LSM6DS3 accelerometer interrupt line on the MSP430 F5529
// LaunchPad using the bit-banging I2C library. The Wire I2C library for
// this LaunchPad defaults to the wrong I2C port, and there is no way to
// change it. The bit-banging I2C library bit-bangs the correct pins, 9 and 10.
StLsm6ds3 myStLsm6ds3(0x6A, &Serial);

#define IRQ_PIN   36

void setup()
{
  Serial.begin(115200);
  I2CBitBang_Init(9, 10);
  myStLsm6ds3.begin();
  pinMode(IRQ_PIN, INPUT);
}

void loop()
{
  Serial.println("Testing ST-LSM6DS3 accelerometer interrupt...");

  while (true)
  {
    Serial.print("IRQ status: ");
    Serial.println(digitalRead(IRQ_PIN));
    delay(1000);
    
    Serial.println("Inverting IRQ pin...");
    myStLsm6ds3.InvertIrq(true);
   
    Serial.print("IRQ status: ");
    Serial.println(digitalRead(IRQ_PIN));
    delay(1000);
    
    Serial.println("Restoring IRQ pin...");
    myStLsm6ds3.InvertIrq(false);
  }
}

