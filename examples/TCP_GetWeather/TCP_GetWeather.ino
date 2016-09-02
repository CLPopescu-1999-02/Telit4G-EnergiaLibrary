#include <Energia.h>
#include <aJSON.h>
#include "LTE_TCP.h"

//////////////////////////////////////////////////////////////////
//       Getting Weather/Location Data with OpenWeatherMap      //
//////////////////////////////////////////////////////////////////
//                                                              //
//  In this example, we use the OpenWeatherMap API to find      //
//  the current temperature and weather at our location.        //
//  This requires you to create a free account online, and      //
//  generate an API key.                                        //
//                                                              //
//  We use the LTE_TCP class to open a socket and send a REST   //
//  request to the OpenWeatherMap endpoint with our home city   //
//  and API key, and use the Energia aJSON library to parse     //
//  the response.                                               //
//                                                              //
//////////////////////////////////////////////////////////////////
 
// COPY YOUR OPEN WEATHER MAP API KEY AND CITY HERE:
#define OWM_API_KEY "f2b7da34048eb1f7ca08d685b30ba314"
//#define OWM_API_KEY "some key"
#define HOME_CITY "Dallas"

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
  Serial.println("UART connection ready.\r\n");
}

void loop()
{
  // Open TCP connection
  if (!lte.socketOpen("api.openweathermap.org")) {
    Serial.println("Failed to open TCP socket.\r\n");
  }
  else {
    // Form an HTTP packet
    char packet[256];
    strcpy(packet, "GET /data/2.5/weather?q=");
    strcat(packet, HOME_CITY);
    strcat(packet, "&appid=");
    strcat(packet, OWM_API_KEY);
    strcat(packet, " HTTP/1.0\r\nConnection: close\r\n\r\n");
    
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

      // First, use socketParseFind() to find the start of the JSON object
      // and skip past the HTTP header information. Then, use the aJSON
      // library to parse the JSON object.
      aJsonObject* owm_json = aJson.parse(lte.socketParseFind("\r\n\r\n"));
      if (owm_json != NULL) {
        aJsonObject* city = aJson.getObjectItem(owm_json, "name");
        aJsonObject* weather = aJson.getObjectItem(owm_json, "weather");
        if (city != NULL && weather != NULL) {
          Serial.print("The weather in ");
          Serial.print(city->valuestring);
          Serial.println(" is:");

          aJsonObject* arrayptr = weather->child;
          while(arrayptr != NULL) {
            Serial.print(aJson.getObjectItem(arrayptr, "description")->valuestring);
            if (arrayptr->next != NULL) Serial.print(", ");
            arrayptr = arrayptr->next;
          }

          Serial.print("The temperature is:");
          aJsonObject* main = aJson.getObjectItem(owm_json, "main");
          Serial.print(((double)(aJson.getObjectItem(main, "temp")->valuefloat) * (9/(double)5)) - 459.67);
          Serial.println(" degrees Fahrenheit");
        }
      } else Serial.println("Error.");
    }

    // Closing socket
    if (!lte.socketClose()) {
      Serial.println("... Failed to close TCP socket.\r\n");
    }
  }

  Serial.println("\r\n");
  delay(10000);
}
