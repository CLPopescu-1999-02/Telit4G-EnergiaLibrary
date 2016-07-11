/* UART loopback between the USB UART and the BoosterPack UART. */

#include <energia.h>

#ifdef __TM4C1294NCPDT__

/* Tiva C TM4C1294 LaunchPad. */

/* Debug port on COM13 (Stellaris Virtual Serial Port). This port is
shared as both the debug port and a virtual UART port. */

// The "Stellaris Virtual Serial Port" USB interface -- COM13.
#define UART_A    Serial

// The HW UART interface on the launchpad 1 connector (by the JTAG).
#define UART_B    Serial7

#else

/* MSP430 F5529 LaunchPad. */

/* Debug port on COM12 (MSP Debug Interface). */

// The "MSP Application UART1" USB interface. COM11.
#define UART_A    Serial

// The HW UART interface on the launchpad.
#define UART_B    Serial1

#endif

#define DEBUG

void setup() {
  UART_A.begin(115200);
  UART_B.begin(115200);

  // Set RTS low. By default, the LE910s enable HW flow control, and
  // without RTS being low, the LE910 will not send any data to us.
#if 1
  UART_A.println("Setting RTS low.");
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);
#else
  // Test case where RTS is an input. This should work, because if the
  // LaunchPad does not drive RTS, then the Telit module should still
  // work, but it doesn't in the case of the 910s.
  UART_A.println("Setting RTS as an input.");
  pinMode(14, INPUT);
#endif


  UART_A.println("UART loop ready.");
#ifdef DEBUG
  UART_A.println("DEBUG mode");
#endif
}

void loop() {
  if (UART_A.available())
  {
    int inByte = UART_A.read();

#ifdef DEBUG
    UART_A.write("Read ");
    UART_A.println(inByte);
#endif

    UART_B.write(inByte);
  }

  if (UART_B.available())
  {
    int inByte = UART_B.read();

#ifdef DEBUG
    UART_A.write("Read ");
    UART_A.println(inByte);
#endif

    UART_A.write(inByte);
  }
}
