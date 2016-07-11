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

#define GPIO_ONOFF     15
#define GPIO_SHUTDOWN  7

void setup() {
  UART_A.begin(115200);
  
  UART_A.println("On/Off and Shutdown with button test.");

  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  
  pinMode(GPIO_ONOFF, OUTPUT);
  pinMode(GPIO_SHUTDOWN, OUTPUT);
}

void loop()
{ 
  int sw1, sw2;

  sw1 = digitalRead(SW1);
  sw2 = digitalRead(SW2);

//  digitalWrite(GPIO_ONOFF, sw1);
//  digitalWrite(GPIO_SHUTDOWN, sw2);

  UART_A.print("ON/OFF: ");
  UART_A.print(sw1);
  UART_A.print(", SHUTDOWN: ");
  UART_A.println(sw2);
}
