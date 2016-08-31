#include <Energia.h>
#include "LTEHttp.h"

// Define UART pins between boosterpack and launchpad
#define LTE_SERIAL Serial1

// Initialize LTEHttp object
LTEHttp lte(&LTE_SERIAL, &Serial);

void setup()
{
  // Initialize UART connections
  Serial.begin(115200);
  LTE_SERIAL.begin(115200);

  //UART_USB.println("Setting RTS low.");
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);

  delay(5000);

  // Initialize LTE Http object
  Serial.println("Initializing...");
  while (!lte.init(4)) {
    Serial.println("Initialization failed. Retrying ...");
    delay(1000);
  }
  Serial.println("Initialization success!");
  Serial.println("UART connection ready.\r\n");
}

void loop()
{
  
  //////////////////////////////////////////////////////////////////
  //                 Transfer data over TCP/IP                    //
  //////////////////////////////////////////////////////////////////
  //                                                              //
  //  This example sends a simple HTTP GET request using a TCP    //
  //  socket, and its related functions. To do this, we open a    //
  //  socket, form an HTTP request, send it over the TCP socket,  //
  //  and read any responses.                                     //
  //                                                              //
  //  The website we're querying is www.httpbin.org, and can be   //
  //  used to test HTTP clients. This specific GET request asks   //
  //  for our own IP address.                                     //
  //                                                              //
  //////////////////////////////////////////////////////////////////
  
  // Open TCP connection
  Serial.println("Opening TCP socket ...");
  if (!lte.socketOpen("www.httpbin.org")) {
    Serial.println("Failed to open TCP socket.\r\n");
  }
  else {
    Serial.println("... Success! Opened TCP connection to \"www.httpbin.org\".\r\n");
    
    // Form an HTTP packet
    char* httpPacket = "GET /ip HTTP/1.1\r\nHost: www.httpbin.org\r\nConnection: close\r\n\r\n";
    Serial.print("HTTP Packet to send:\r\n");
    Serial.println(httpPacket);
    Serial.println("");
    
    // Send HTTP packet
    Serial.println("Sending Packet ...");
    if (lte.socketWrite(httpPacket) == -1) {
      Serial.println("... Failed to send.\r\n");
    } else {
      Serial.println("... Success!\r\n");
    }

    // Wait for a response
    delay(5000);

    // Receive a response
    Serial.println("Attempting to receive response ...");
    int num = lte.socketReceive();
    if (num < 0) {
      Serial.println("... Error in receiving data.\r\n");
    } else {
      Serial.print("... Success! ");
      Serial.print(num);
      Serial.println(" bytes received.");
      Serial.println("Received data: ");
      Serial.println(lte.receivedData());
    }

    // Closing socket
    Serial.println("Closing socket ...");
    if (lte.socketClose()) {
      Serial.println("... Success!\r\n");
    } else {
      Serial.println("... Failed to close TCP socket.\r\n");
    }
  }

  Serial.println("\r\n");
  delay(10000);
}
