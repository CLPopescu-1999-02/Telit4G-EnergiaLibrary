/* Tests the cullular module on/off on the Telit BoosterPack. */

/* The SHUTDOWN pin is an emergency power down and should only be used if the
 module is not responding to the on/off command. It is active high, and so to
 shutdown the module, */
 
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

#define GPIO_ONOFF     15
#define GPIO_SHUTDOWN  7

#define LED1  40
#define LED2  39
#define LED3  38

void setup() {  
  UART_A.begin(115200);
  
  UART_A.println("Cellular module on/off and shutdown test.");
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  
  digitalWrite(GPIO_ONOFF, HIGH);
  pinMode(GPIO_ONOFF, OUTPUT);
  
  /*
  digitalWrite(GPIO_SHUTDOWN, LOW);
  pinMode(GPIO_SHUTDOWN, OUTPUT);
  */
  
  UART_A.println("Waiting 10 seconds...");
  delay(10000);
}

void loop()
{  
  UART_A.println("Turning off cellular module...");
  digitalWrite(GPIO_ONOFF, LOW);
  delay(25);
  digitalWrite(GPIO_ONOFF, HIGH);  

UART_A.println("Waiting forever...");
while (1);

  UART_A.println("Waiting 10 seconds...");
  delay(10000);
  
  UART_A.println("Turning on cellular module...");
  digitalWrite(GPIO_ONOFF, LOW);
  delay(25);
  digitalWrite(GPIO_ONOFF, HIGH);  

  UART_A.println("Waiting 10 seconds...");
  delay(10000);

  UART_A.println("Turning off cellular module...");
  digitalWrite(GPIO_ONOFF, LOW);
  delay(25);
  digitalWrite(GPIO_ONOFF, HIGH);  

  UART_A.println("Waiting 10 seconds...");
  delay(10000);
  
  UART_A.println("Turning on cellular module...");
  digitalWrite(GPIO_ONOFF, LOW);
  delay(25);
  digitalWrite(GPIO_ONOFF, HIGH);  

  UART_A.println("Waiting 10 seconds...");
  delay(10000);

/*
  UART_A.println("Using SHUTDOWN to turn off cellular module...");
  digitalWrite(GPIO_SHUTDOWN, HIGH);

  UART_A.println("Waiting 10 seconds...");
  delay(10000);

  digitalWrite(GPIO_SHUTDOWN, LOW);
  delay(25);

  UART_A.println("Turning on cellular module...");
  digitalWrite(GPIO_ONOFF, HIGH);
  delay(25);
  digitalWrite(GPIO_ONOFF, LOW);

  UART_A.println("Waiting 10 seconds...");
  delay(10000);
  */
}
