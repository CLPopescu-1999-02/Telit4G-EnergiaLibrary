/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * 4G/LTE Library for Telit LE910SV module and Energia.
 *
 * This library is adapted from the GSM Arduino library provided by
 * Justin Downs (2010) at
 * http://wiki.groundlab.cc/doku.php?id=gsm_arduino_library
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


#ifndef LTE_LTEBASE_
#define LTE_LTEBASE_

#include "LTEBase.h"

/** LTE Base class constructor.
 *
 *  @param  tp  Telit Serial port.
 *  @param  dp  Debug Serial port pointer.
 */
LTEBase::LTEBase(HardwareSerial& tp, HardwareSerial* dp) {
  telitPort = tp;
  debugPort = dp;
}

// TODO: finish
/** Selects frequency band that Telit uses to communicate.
 *
 *  @param  _band Frequency band used by Telit.
 *  @return bool  True on success.
 */
bool LTEBase::init(uint16_t _band) {
  #ifdef DEBUG
  debugPort->write("Setting frequency ...\r");
  #endif

  //TODO: look up AT commands to set frequency bands

  return true;
}



#endif
