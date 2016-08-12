#include <stdio.h>
#include <assert.h>

#include "LTEBase.h"
#include "LTEHTTP.h"

int main() {
  HardwareSerial telit_serial;
  HardwareSerial debug_serial;
  telit_serial.begin(9600);
  debug_serial.begin(9600);

  initmillis();

  ///////////////////////////////////////////////////////////
  //                  Test LTE Base                        //
  ///////////////////////////////////////////////////////////

  LTEBase base(telit_serial, &debug_serial);
  
  // quick serial test
  /*
  char buf[10];
  int x = 0;
  telit_serial.write("asdf\r\n");
  while (telit_serial.available()) {
    printf("%d\n", x);
    buf[x] = telit_serial.read();
    x++;
  }
  buf[x] = '\0';
  printf(buf);
  */

  base.receiveData(10000, 60);

  //fprintf(stderr, base.getData());
  //assert(!base.receiveData(10000, 60));

  // Telit communication
  //assert(base.sendATCommand(""));
  //assert(!base.sendATCommand("", -999999));
  //assert(!base.sendATCommand("", -1));
  //assert(!base.sendATCommand("", 0));
  //assert(base.sendATCommand("", 1));
  //assert(base.sendATCommand("", 999999));

  fprintf(stderr, "Tests Succeeded\n");
}
