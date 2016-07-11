/* Tests the potentiometers on the Telit BoosterPack. */

#include <energia.h>

#ifdef __TM4C1294NCPDT__

/* Tiva C TM4C1294 LaunchPad. */

/* Debug port on COM13 (Stellaris Virtual Serial Port). This port is
shared as both the debug port and a virtual UART port. */

// The "Stellaris Virtual Serial Port" USB interface -- COM13.
#define UART_A    Serial

#else

/* MSP430 F5529 LaunchPad. */

/* Debug port on COM12 (MSP Debug Interface). */

// The "MSP Application UART1" USB interface. COM11.
#define UART_A    Serial

#endif

#define POT1  2
#define POT2  6

void setup() {
  UART_A.begin(115200);
  UART_A.println("Potentiometer test.");
}

void loop()
{ 
  Serial.print("Pot1: ");
  Serial.print(analogRead(POT1));
  Serial.print(", Pot2: ");
  Serial.println(analogRead(POT2));
}
