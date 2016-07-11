#include <gsmbase.h>
#include <gsmMaster.h>
#include <gsmGPRS.h>
#include <gsmSMS.h>
#include <I2CBitBang.h>
#include <StLsm6ds3.h>
#include <StLps25h.h>
#include <TiHdc1050.h>

// Define UART pins between usb and BoosterPack
#define UART_USB Serial
#define UART_BP Serial1

gsmGPRS  myGsmGPRS(UART_BP,&millis,&UART_USB);

void setup()
{
  // put your setup code here, to run once:
  UART_USB.begin(115200);
  UART_BP.begin(115200);

  UART_USB.println("Setting RTS low.");
  pinMode(14, OUTPUT);
  digitalWrite(14, 0);

  UART_USB.println("UART connection ready.");


  myGsmGPRS.init(3);               // set for New York NY
  UART_USB.println("done init");
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (UART_USB.available())
  {
    UART_USB.read();
    UART_BP.write("AT+CGSN\r");
  }

  if (UART_BP.available())
  {
    int inByte = UART_BP.read();

    UART_USB.write("Read: ");
    UART_USB.print(inByte);
    UART_USB.print(" ");
    UART_USB.write(inByte);
    UART_USB.println(" ");
  }
}
