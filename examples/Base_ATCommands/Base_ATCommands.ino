#include <Energia.h>
#include "LTE_Base.h"

// Define UART pins between boosterpack and launchpad
#define LTE_SERIAL Serial1

// Initialize LTE_Base object
LTE_Base base(&LTE_SERIAL, &Serial);

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
  // check connection 
  if (base.isConnected())
    Serial.println("This BoosterPack is connected to the network!");

  //////////////////////////////////////////////////////////////////
  //                Send the modem an AT command                  //
  //////////////////////////////////////////////////////////////////
  //
  //  *** What is an AT command? ***
  //  AT commands are instructions used to control a modem. They can
  //  be used to obtain information about the modem, make/receive
  //  calls and texts, open a data connection, etc. You can find more
  //  information about specific AT commands in the Telit AT command
  //  guide:
  //  http://www.telit.com/fileadmin/user_upload/products/Downloads/4G/Telit_LE910_V2_Series_AT_Commands_Reference_Guide_r2.pdf
  //
  //  You can send an AT command one of two ways:
  //    1) using getCommandOK(), or
  //    2) using both sendATCommand() and receiveData()
  //  
  //  *** bool getCommandOK(const char* command) ***
  //  This function handles both the sending and receiving of data.
  //  It sends the command it's given as an argument to the modem, and
  //  listens for a response. If the response contains the word "OK",
  //  the BoosterPack confirms that it has received our command, and
  //  the function returns true. The function waits 2 seconds for a
  //  response by default.
  //  
  //  *** bool sendATCommand(const char* command) ***
  //  *** bool receiveData(uint timeout, uint baudDelay) ***
  //  These two functions send data to the BoosterPack modem, and
  //  listen for a response respectively. The sendATCommand() accepts
  //  a string argument. The receiveData() function accepts two
  //  arguments - timeout is the amount of time (in ms) to wait for any
  //  sort of data from the modem, while baudDelay (ms) is the maximum
  //  time between received bytes once the modem begins sending data.
  //  In order to catch the response from the BoosterPack, receiveData()
  //  must be called after the command is sent. These two commands allow
  //  you to be more flexible in how you send/wait for a response.
  //  For example, in the following code:
  //      sendATCommand("AT+Command1");
  //      sendATCommand("AT+Command2");
  //      receiveData(300,1000);
  //  We don't care about waiting for a response frmo the first AT command,
  //  only the second. In addition, because the timeout value is 300ms, we
  //  expect a quick response from the modem. Because the baudDelay is high,
  //  we continue to wait for data transmission for a long time after the
  //  initial connection is established - even if received data bytes are
  //  few and far between. This is perfect for slower/inconsistent connections.
  //
  //
  //  After sending a command, you can access the response with char* getData().
  //  You can also find specific strings in the response with the functions
  //  bool parseFind(char *) and char* getParsedData()
  //  
  //////////////////////////////////////////////////////////////////
  
  // Set command echo on
  Serial.println("Setting command echo on ...");
  if (base.getCommandOK("ATE1"))
    Serial.println(" > Command echo is now on.");
  else Serial.println(" > Command Failed.");

  // Get manufacturer ID
  Serial.println("Requesting manufacturer identification ...");
  if (base.sendATCommand("AT+CGMI") && base.receiveData())
    Serial.println(" > Received a response to AT command \"AT+CGMI\" (manufacturer ID).");
  else Serial.println(" > Command failed");

  // Find out if calls are made using data or voice
  Serial.println("Finding out if calls are made with data or voice ...");
  Serial.println("Sending AT command \"AT+FCLASS\" ...");
  if (base.getCommandOK("AT+FCLASS?")) {
    Serial.println(" > Received a response to AT+FCLASS?");
    Serial.println(" > Response:");
    Serial.println(base.getData());
    Serial.println(" > End response.");
    
    Serial.println("Parsing data from response:");
    base.parseFind("AT+FCLASS?\r\n\r\n");
    if (base.getParsedData()[0] == '0') {
      Serial.println(" > Calls are made with data by default. (AT+FCLASS=0)");
    }
    else if (base.getParsedData()[0] == '8') {
      Serial.println(" > Calls are made with voice by default. (AT+FCLASS=8)");
    }
  }
  else Serial.println(" > Command failed");
  
  Serial.println("");
  Serial.println("");
  
  delay(20000);
}
