#include <Energia.h>

#include "LTE_TCP.h"

//////////////////////////////////////////////////////////////////
//                   Simple HTTP POST request                   //
//////////////////////////////////////////////////////////////////
//                                                              //
//  In this example, we use an HTTP POST request to send info-  //
//  rmation about ourselves to the website Requestb.in. There,  //
//  we can view information about the packets we sent.          //
//                                                              //
//  In order to run this example, you need to visit the website //
//  http://requestb.in and create a request bin. Set the        //
//  URL_RESOURCE to the string after requestb.in/ Afterwards,   //
//  set the NAME variables to your first and last name.         //
//                                                              //
//  After you run the program, you can navigate to your         //
//  http://requestb.in/URL_RESOURCE to see what information     //
//  you sent.                                                   //
//                                                              //
//////////////////////////////////////////////////////////////////
 
// COPY YOUR OPEN WEATHER MAP API KEY AND CITY HERE:
#define URL_RESOURCE "138a3p81"
#define FIRST_NAME "Wenlong"
#define LAST_NAME "Xiong"

// Define UART pins between boosterpack and launchpad
#define LTE_SERIAL Serial1

// Initialize LTEHttp object
LTE_TCP lte(&LTE_SERIAL, &Serial);

void setup()
{
  // Initialize UART connections
  Serial.begin(115200);
  LTE_SERIAL.begin(115200);

  //UART_USB.println("Setting RTS low.");
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);

  delay(2000);

  // Initialize LTE Http object
  Serial.println("Initializing...");
  while (!lte.init(4)) {
    Serial.println("Initialization failed. Retrying ...");
    delay(1000);
  }
  Serial.println("Initialized. UART connection ready.\r\n");
}

void loop()
{
  // Open TCP connection
  if (!lte.socketOpen("requestb.in")) {
    Serial.println("Failed to open TCP socket.\r\n");
  }
  else {
    // Form an HTTP packet
    char packet[256];
    strcpy(packet, "POST /");
    strcat(packet, URL_RESOURCE);
    strcat(packet, " HTTP/1.1\r\nConnection: close\r\nHost: requestb.in\r\n");
    strcat(packet, "Content-Length: ");
    char s[64];
    sprintf(s, "%d", strlen(NAME) + 5);
    strcat(packet, s);
    strcat(packet, "\r\n\r\n");
    strcat(packet, "firstname=");
    strcat(packet, FIRST_NAME);
    strcat(packet, "&");
    strcat(packet, "lastname=");
    strcat(packet, LAST_NAME);
    strcat(packet, "\r\n\r\n");
    
    Serial.print("HTTP Packet:\r\n");
    Serial.print(packet);
    
    // Send HTTP packet
    if (lte.socketWrite(packet) == -1) {
      Serial.println("... Failed to send.\r\n");
    }

    // Wait for a response
    delay(5000);

    // Receive a response
    int num = lte.socketReceive();
    if (num < 0) {
      Serial.println("... Error in receiving data.\r\n");
    } else {
      Serial.println("Response received.");
      Serial.println(lte.getReceivedData());
    }

    // Closing socket
    if (!lte.socketClose()) {
      Serial.println("... Failed to close TCP socket.\r\n");
    }
  }

  Serial.println("\r\n");
  delay(10000);
}
