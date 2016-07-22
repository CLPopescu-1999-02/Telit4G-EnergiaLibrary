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


#ifndef LTE_LTE_HTTP_
#define LTE_LTE_HTTP_

#include "LTE_Base.h"
#include "LTE_Http.h"


/** LTE Http class constructor.
 *
 *  @param  tp  Telit Serial port.
 *  @param  dp  Debug Serial port pointer.
 */
LTEHttp::LTEHttp(HardwareSerial& tp, HardwareSerial* dp) : LTEBase::LTEBase(tp, dp) {
}

/** Clears IP addresses, and selects frequency band that Telit
 *  uses to communicate.
 *
 *  @param  lte_band    Frequency band used by Telit.
 *  @return bool        True on success.
 */
bool LTEHttp::init(uint16_t lte_band){
  LTEBase::init(lte_band);
  //memset(hostIP, '\0', );
  return false;
}

#endif
