/* Tests the two push buttons on the Telit BoosterPack. */

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

#define SW1  5
#define SW2  8

void setup() {
  UART_A.begin(115200);
  UART_A.println("Button test.");
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
}

void loop()
{ 
  UART_A.print("Switch 0: ");
  UART_A.print(digitalRead(SW1));
  UART_A.print(", Switch 1: ");
  UART_A.println(digitalRead(SW2));
}
