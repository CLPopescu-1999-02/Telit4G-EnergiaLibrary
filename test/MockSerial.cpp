/*
 * Mock Serial class to replace the Energia one in unit testing
 *
 */

#ifndef MOCKSERIAL__
#define MOCKSERIAL__

#include "MockSerial.h"

void HardwareSerial::begin(int speed) {
  read_index = 0;
  write_index = 0;
  bufsize = 0;
};

// Overloaded write functions
int HardwareSerial::write(int x) {
  if (!append_buf(x)) return 0;
  return 1;
}

int HardwareSerial::write(const char* x, int len) {
  int count = 0;
  for (int k = 0; k < len; k++) {
    if(!append_buf(x[k])) return count;
    count++;
  }
  return count;
}

int HardwareSerial::write(std::string str) {
  int len = str.length();
  const char* x = str.c_str();
  return write(x, len);
}

int HardwareSerial::read() {
  if ((bufsize <= 0) || (read_index >= BUFFER_SIZE))
      return -1;
  int ret = buf[read_index];
  read_index++;
  bufsize--;
  return ret;
}

int HardwareSerial::available() {
  return bufsize;
}

bool HardwareSerial::append_buf(int i) {
  if (bufsize >= BUFFER_SIZE) return false;
  if (write_index >= BUFFER_SIZE) write_index = 0;
  buf[write_index] = i;
  write_index++;
  bufsize++;
  return true;
}

void HardwareSerial::clear() {
  bufsize = 0;
  read_index = 0;
  write_index = 0;
}

#endif
