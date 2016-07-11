/* Tests the LEDs on the Telit BoosterPack. */

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

#define LED1  40
#define LED2  39
#define LED3  38

void setup() {
  UART_A.begin(115200);
  Serial.begin(9600);
  UART_A.println("LED test.");
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop()
{ 
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED1, LOW);
  delay(200);
  
  Serial.println("led1");
  
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED2, LOW);
  delay(200);
  
  Serial.println("led2");

  digitalWrite(LED3, HIGH);
  delay(200);
  digitalWrite(LED3, LOW);
  delay(200);
  
  Serial.println("led3");
}
