#include <Wire.h>
#include <I2CBitBang.h>
#include <StLsm6ds3.h>

#include <aJSON.h>
#include <Energia.h>

#include "LTE_TCP.h"

///////////////////////////////////////////////////////////////////
//      Using IBM Bluemix & Twilio & Onboard Sensors for         //
//             Motion-Triggered Text Alerts                      //
///////////////////////////////////////////////////////////////////
//                                                               //
//  This example uses IBM Bluemix, Twilio API, Node-RED IDE,     //
//  and the EVK4's onboard gyroscope to have a text message      //
//  sent to your phone whenever your launchpad is moved. We      //
//  also use the onboard sensors to measure temperature.         //
//                                                               //
//  *** Set up IBM Bluemix ***                                   //
//  1) Create a free account at www.bluemix.net                  //
//  2) Catalog > Boilerplates > Node-RED Starter                 //
//     i. Name your app                                          //
//    ii. Fill in your app name in the define below              //
//  3) Go to your application                                    //
//                                                               //
//  *** Set up Twilio ***                                        //
//  1) Create free account at www.twilio.com                     //
//  2) Get free phone number                                     //
//  3) Find Account SID and Authentication Token in Dashboard    //
//                                                               //
//  *** Node-RED editor ***                                      //
//  1) Create the following nodes in the editor, and             //
//     connect them as shown                                     //
//    [http (input)] ----> [http response (output)]              //
//         |                                                     //
//         V                                                     //
//    [function] --> [template] --> [twilio (output)]            //
//                                                               //
//  2) Edit the nodes to have the following values:              //
//  [http (input)]                                               //
//    * Method: GET                                              //
//    * URL: /sensor                                             //
//  [function]                                                   //
//    * Function:                                                //
//      msg.payload.movement = msg.payload.movement.toLowerCase()//
//      if (msg.payload.movement === "true") {                   //
//        return msg;                                            //
//      }                                                        //
//    * Outputs: 1                                               //
//  [template]                                                   //
//    * Set property: msg.payload                                //
//    * Template: This is the contents of the text message.      //
//    * Format: Mustache template                                //
//  [twilio]                                                     //
//    * Twilio: Add new twilio API ... (and fill in the Account  //
//              SID, Authentication Token, and your Twilio       //
//              Phone number from the previous step)             //
//    * SMS to: Receiving phone number                           //
//                                                               //
//                                                               //
//  Make sure your Bluemix application is started and running    //
//  before running this Energia sketch.                          //
//                                                               //
///////////////////////////////////////////////////////////////////

// COPY YOUR BLUEMIX APP NAME HERE
// For example, the APP_NAME of myapp.bluemix.net would be "myapp"
#define APP_NAME "evk4temperaturemovement"

// Define UART pins between boosterpack and launchpad
#define LTE_SERIAL Serial1

// Initialize LTEHttp object
LTE_TCP lte(&LTE_SERIAL, &Serial);

// Initialize onboard sensors
StLsm6ds3 myStLsm6ds3(0x6A, &Serial);
long long last_temp;
long long last_moved;
bool moving;

void setup()
{
  // Initialize UART connections
  Serial.begin(115200);
  LTE_SERIAL.begin(115200);
  Wire.begin();
  myStLsm6ds3.begin();
  
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

  last_temp = millis();
  last_moved = millis();
  bool moving = false;
}

void loop()
{
  float temp, accel[3], gyro[3];
  // Read temperature and gyroscope data from device
  myStLsm6ds3.readTemperature(temp);
  myStLsm6ds3.readGyro(gyro);

  // If sufficient movement, then tell your Bluemix App to 
  // send a text message.
  if ((abs(gyro[0]) + abs(gyro[1]) + abs(gyro[2])) > 20) {
    Serial.println("Someone's moving your EVK4!");
    if (!moving) {
      Serial.println("Sending text via Twilio ...");
      char remoteURL[256];
      //strcpy(remoteURL, "http://");
      strcpy(remoteURL, APP_NAME);
      strcat(remoteURL, ".mybluemix.net");
      char* resource = "/sensor?movement=true";

      // Send request, get response
      char* response = sendHttpGet(remoteURL, resource);

      // Parse response into JSON object
      Serial.println("Response received:");
      aJsonObject* response_json = aJson.parse(strstr(response, "{"));
      Serial.println(aJson.print(response_json));
    }
    moving = true;
    last_moved = millis();
  }

  // Delay, so that continuous motion doesn't continuously send texts
  if (millis() - last_moved > 2000) {
    moving = false;
  }

  // Prints current temperature in both Celcius and
  // Fahrenheit every 10 seconds
  if ((millis() - last_temp) > 10000) {
    last_temp = millis();
    Serial.print("The current temperature is ");
    Serial.print(temp);
    Serial.print(" degrees Celcius, or ");
    Serial.print(((temp*9.0/5.0)) + 32.0);
    Serial.println(" degrees Fahrenheit.");
  }

  delay(100);
}

// Returns response data from HTTP GET request
char* sendHttpGet(char* remoteURL, char* resource) {
  int tries = 0;
  while(!lte.socketOpen(remoteURL) && tries < 5) tries++;
  if (tries >= 5) {
    Serial.println("Failed to open connection.");
    return "";
  }
  Serial.println("Connection opened.");

  char packet[256];
  strcpy(packet, "GET ");
  strcat(packet, resource);
  strcat(packet, " HTTP/1.1\r\nConnection: close\r\nHost: ");
  strcat(packet, remoteURL);
  strcat(packet, "\r\n");
  strcat(packet, "Content-Length: 0\r\n\r\n");

  if (lte.socketWrite(packet) == -1) {
    Serial.println("... Failed to send.");
    return "";
  }

  delay(2000);

  if (lte.socketReceive() <= 0) Serial.println("... Error in receiving.");
  char* ret = lte.getReceivedData();

  // Closing socket
  while(!lte.socketClose()) {
    Serial.println("... Failed to close socket.");
    delay(200);
  }

  return ret;
}
