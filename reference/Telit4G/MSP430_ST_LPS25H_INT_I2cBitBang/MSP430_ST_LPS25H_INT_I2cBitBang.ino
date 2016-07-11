#include <StLps25h.h>
#include <I2CBitBang.h>

// Tests the ST LPS25H temperature and pressure sensor interrupt on the MSP430 F5529
// LaunchPad using the bit-banging I2C library. The Wire I2C library for this LaunchPad
// defaults to the wrong I2C port, and there is no way to change it. The bit-banging I2C
// library bit-bangs the correct pins, 9 and 10.

StLps25h myStLps25h(0x5C, &Serial);

#define IRQ_PIN         35

#define GPIO_ONOFF      15
#define GPIO_SHUTDOWN   7

void setup()
{
  Serial.begin(115200);
  I2CBitBang_Init(9, 10);
  myStLps25h.begin();
  pinMode(IRQ_PIN, INPUT);

  digitalWrite(GPIO_ONOFF, HIGH);
  pinMode(GPIO_ONOFF, OUTPUT);
}

void loop()
{
  static int count=0;
  
  Serial.println("Testing ST-LPS25H temperature/humidity sensor...");

  while (true)
  {
    Serial.print("IRQ status: ");
    Serial.println(digitalRead(IRQ_PIN));
    delay(1000);
    
    Serial.println("Inverting IRQ pin...");
    myStLps25h.InvertIrq(true);
   
    Serial.print("IRQ status: ");
    Serial.println(digitalRead(IRQ_PIN));
    delay(1000);
    
    Serial.println("Restoring IRQ pin...");
    myStLps25h.InvertIrq(false);
    
    count++;
    if (count == 5)
    {
      Serial.println("Turning off cellular module...");
      digitalWrite(GPIO_ONOFF, LOW);
      delay(25);
      digitalWrite(GPIO_ONOFF, HIGH);  
    }
  }
}

