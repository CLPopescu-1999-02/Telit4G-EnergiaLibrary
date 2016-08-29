/*
 * Mock Serial class to replace the Energia one in unit testing
 *
 */

#ifndef MOCKSERIAL_H__
#define MOCKSERIAL_H__

#include <stdio.h>
#include <string>

#define BUFFER_SIZE 100000  // Max buffer size

class HardwareSerial {
public:
  void begin(int speed);
  void end() { };

  // Overloaded write functions
  int write(int x);
  int write(std::string);
  int write(const char* x, int len);
  int write(uint8_t* x, int len);

  // other basics
  int read();
  int available();

  // for testing purposes
  void clear();
  
private:
  bool append_buf(int i);
  int buf[BUFFER_SIZE];
  int write_index;
  int read_index;
  int bufsize;
};

#endif
