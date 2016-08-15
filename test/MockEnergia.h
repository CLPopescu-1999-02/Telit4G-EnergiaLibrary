#ifndef MOCKENERGIA_H__
#define MOCKENERGIA_H__

#include <sys/time.h>
#include <stdint.h>

#include "MockSerial.h"

static struct timeval stop, start;

static void initmillis() {
  gettimeofday(&start, NULL);
}

static uint32_t millis() {
  gettimeofday(&stop, NULL);
  return (stop.tv_usec - start.tv_usec);
}

#endif
