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

#define GPIO4  32
#define GPIO5  31
#define GPIO6  19
#define GPIO7  18
#define GPIO8  37
#define GPIO9  17
#define GPIO10 16

void setup() {
  UART_A.begin(115200);
  
  UART_A.println("GPIO 4-10 test.");
}

void loop()
{ 
  UART_A.print("GPIO4: ");
  UART_A.print(digitalRead(GPIO4));
  UART_A.print(", GPIO5: ");
  UART_A.print(digitalRead(GPIO5));
  UART_A.print(", GPIO6: ");
  UART_A.print(digitalRead(GPIO6));
  UART_A.print(", GPIO7: ");
  UART_A.print(digitalRead(GPIO7));
  UART_A.print(", GPIO8: ");
  UART_A.print(digitalRead(GPIO8));
  UART_A.print(", GPIO9: ");
  UART_A.print(digitalRead(GPIO9));
  UART_A.print(", GPIO10: ");
  UART_A.println(digitalRead(GPIO10));
}
