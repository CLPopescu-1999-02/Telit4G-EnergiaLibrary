#include <Energia.h>
#include "LTEBase.h"

// Define UART pins between boosterpack and launchpad
#define LTE_SERIAL Serial1

// Initialize LTEBase object
LTEBase base(&LTE_SERIAL, &Serial);

void setup()
{
  // Initialize UART connections
  Serial.begin(115200);
  LTE_SERIAL.begin(115200);

  //UART_USB.println("Setting RTS low.");
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);

  // Initialize LTE Base
  Serial.println("Initializing...");
  if (!base.init(4)) {
    Serial.println("Initialization failed");
  }
  Serial.println("Initialization success!");
  
  Serial.println("UART connection ready.\r\n");
}

void loop()
{
  base.printRegistration();
  
  Serial.println("");

  // print registration information every 2 seconds
  delay(5000);
}
