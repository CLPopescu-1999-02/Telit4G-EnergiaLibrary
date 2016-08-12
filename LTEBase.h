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

// Comment this out to remove debug messages
// #define DEBUG

// Uncomment this to enable unit testing
#define UNIT_TEST

#include <stdlib.h>
#include <string.h>

#ifdef UNIT_TEST
#include "MockEnergia.h"
#else
#include <Energia.h>
#endif

class LTEBase {
public:
    // Basic setup
    LTEBase(HardwareSerial &telitPort, HardwareSerial* debugPort);
    virtual bool init(uint32_t lte_band);

    // Only used if you turn on the Telit module using the LaunchPad
    //virtual bool turnOn();
    //virtual bool turnOff();

    // Telit communication
    // Basic functions
    virtual bool sendATCommand(const char*, uint32_t timeout = 10000,
                             uint32_t baudDelay = 60);
    virtual bool receiveData(uint32_t timeout, uint32_t baudDelay);
    virtual char* getData() { return data; };
    virtual char* getParsedData() { return parsedData; };
    // More abstracted functions
    virtual bool parseFind(const char*);  /* Search for substring in data */
    bool getCommandOK(const char*);  /* Send command, verify OK response */

    // Basic commands for Telit module
    virtual void printRegistration();  /* Prints serial numbers */
    virtual bool isConnected();  /* Connection status */


protected:
    HardwareSerial telitPort;  /* Telit serial interface */
    HardwareSerial* debugPort = NULL;  /* Pointer so it can default to null */
    char* data;  /* Response data from Telit */
    uint32_t recDataSize;  /* Size of response data from Telit */
    char* parsedData;  /* Parsed response data */
};

#endif
