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


#ifndef LTE_LTE_BASE_
#define LTE_LTE_BASE_

#include "LTE_Base.h"


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
 *  @param  _band   Frequency band used by Telit.
 *  @return bool    True on success.
 */
bool LTEBase::init(uint16_t _band) {
    #ifdef DEBUG
    debugPort->write("Setting frequency band ...\r");
    #endif

    //TODO: look up AT commands to set frequency bands
    //sendATCommand("");

    return true;
}


// TODO: test
/** Sends an AT command to the Telit module, and lets the user know
 *  if there is a response.
 *
 *  @param  cmd   String containing AT command to send.
 *  @return bool  True if there is a response from the module,
 *                false otherwise.
 */
bool LTEBase::sendATCommand(const char* cmd) {
    #ifdef DEBUG
    debugPort->write("Sending AT Command: ");
    debugPort->write(cmd);
    debugPort->write(" ...\r");
    #endif

    telitPort.write(cmd);
    telitPort.write("\r");

    return receiveData();
}


// TODO: test
/** Listens to the serial port for data from the Telit module and captures
 *  it in the char* data field. Returns false if at any point communication
 *  times out.
 *
 *  @param  timeout
 *  @return bool
 */
bool LTEBase::receiveData(uint32_t timeout, uint32_t baudDelay) {
    // Initialize receive buffer
    int dataSize = 20;  /* Initial size (in chars) of buffer used to
                                 store received data (default is 20) */
    char* receiveBuf = (char*) malloc(dataSize * sizeof(char));
    memset((void*) receiveBuf, '\0', dataSize);
    if (receiveBuf == NULL) {  /* No more memory to allocate */
        return false;
    }

    // Block while waiting for the start of the message
    uint16_t startTime = millis();
    while (telitPort.available() < 1) {
        if ((millis() - startTime) > timeout) {
            return false;  /* Timeout */
        }
    }

    // Receive data from serial port
    uint16_t dataPos = 0;
    startTime = millis();
    bool timedOut = false;
    while (!timedOut) {
        // Expand buffer if it's full
        if ((dataPos + 1) > dataSize) {
            dataSize *= 2;
            realloc(receiveBuf, dataSize);
        }

        // Store next byte
        receiveBuf[dataPos] = telitPort.read();
        dataPos++;
        startTime = millis();

        // Wait for more data
        while (telitPort.available() < 1) {
            if ((millis() - startTime) > baudDelay) {
                receiveBuf[dataPos] = '\0';  /* Null terminate */
                dataPos++;
                timedOut = true;
                break;
            }
        }
    }

    // Store received data
    if (data != NULL) {
        free(data);
    }
    if (!timedOut) {
        data = (char*) malloc((dataPos) * sizeof(char));
        memcpy(data, receiveBuf, dataPos);
        dataSize = dataPos;
    } else {
        data = NULL;
        dataSize = 0;
    }
    free(receiveBuf);

    #ifdef DEBUG
    debugPort->write("Received Data:\r")
    debugPort->write(receiveBuf);
    debugPort->write("\r");
    #endif

    return timedOut ? false : true;
}

#endif
