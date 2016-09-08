#include <Energia.h>
#include "LTE_TCP.h"

// Define UART pins between boosterpack and launchpad
#define LTE_SERIAL Serial1

// Initialize LTE_TCP object
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
  if (!lte.init(4)) {
    Serial.println("Initialization failed");
  }
  else Serial.println("Initialization success!");
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
  //  The website we're querying is www.energia.nu, and the       //
  //  resource is /hello. This page returns a basic website with  //
  //  the text "Hello, World."                                    //
  //                                                              //
  //////////////////////////////////////////////////////////////////
  
  // Open TCP connection
  Serial.println("Opening TCP socket ...");
  if (!lte.socketOpen("www.energia.nu")) {
    Serial.println("Failed to open TCP socket.\r\n");
  }
  else {
    Serial.println("... Success! Opened TCP connection to \"www.energia.nu\".\r\n");
    
    // Form an HTTP packet
    char* httpPacket = "GET /hello HTTP/1.1\r\nHost: www.energia.nu\r\nConnection: close\r\n\r\n";
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
    delay(2000);

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
