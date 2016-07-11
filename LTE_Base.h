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


#ifndef LTE_LTE_BASE_H_
#define LTE_LTE_BASE_H_

#include <stdlib.h>
#include <string.h>

#include <Energia.h>


class LTEBase {
  public:
    // Basic setup
    LTEBase(HardwareSerial &telitPort, HardwareSerial* debugPort);
    virtual bool init(uint16_t _band);

    // Only used if you turn on the Telit module using the LaunchPad
    virtual bool turnOn();
    virtual bool turnOff();

    // Telit communication
    virtual bool sendATCommand(const char*);
    virtual bool receiveData(uint32_t timeout = 180000,
                             uint32_t baudDelay = 60);
    virtual char* getData() { return data; };
    virtual char* getParsedData() { return parsedData; }

    // Basic commands for Telit module
    virtual const char* printRegistration();  /* Prints serial numbers */
    virtual bool isConnected();  /* Connection status */

  protected:
    HardwareSerial telitPort;  /* Telit serial interface */
    HardwareSerial* debugPort = NULL;  /* Pointer so it can default to null */
    char* data;  /* Response data from Telit */
    uint16_t dataSize;  /* Size of response data from Telit */
    char* parsedData;  /* Parsed response data */
};

#endif
