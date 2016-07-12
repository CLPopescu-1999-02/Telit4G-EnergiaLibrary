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

// TODO: test
/** Sets up initial settings, and selects frequency band that Telit
 *  uses to communicate.
 *
 *  @param  lte_band   Frequency band used by Telit.
 *  @return bool    True on success.
 */
bool LTEBase::init(uint16_t lte_band) {
    #ifdef DEBUG
    debugPort->write("Initializing ...\r");
    #endif

    //TODO: look up AT commands to set frequency bands

    // Set no echo
    if (!sendATCommand("ATE0", 500, 2000)) return false;

    if (!getCommandOK("ATV1")) return false;  /* Verbose response */
    if (!getCommandOK("AT+IPR=115200")) return false;  /* Baud rate */
    if (!getCommandOK("AT+CMEE=2")) return false;  /* Verbose error reports */
    if (!getCommandOK("AT&K0")) return false;  /* No flow control */

    /* If you are using a 2G/3G capable device, you would change
     * The arguments here to include your GSM and UMTS bands. For the Telit
     * EVK4 (LE910SV-V2), _band can be 4 or 13. */
    char* band = "";
    int gsm_band = 0;
    int umts_band = 0;
    sprintf(band, "AT#BND=%d,%d,%d", gsm_band, umts_band, lte_band);
    if (!getCommandOK(band)) return false;

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
bool LTEBase::sendATCommand(const char* cmd, uint32_t timeout,
                            uint32_t baudDelay) {
    #ifdef DEBUG
    debugPort->write("Sending AT Command: ");
    debugPort->write(cmd);
    debugPort->write(" ...\r");
    #endif

    telitPort.write(cmd);
    telitPort.write("\r");

    return receiveData(timeout, baudDelay);
}

// TODO: test
/** Listens to the serial port for data from the Telit module and captures
 *  it in the char* data field. Returns false if at any point communication
 *  times out.
 *
 *  @param  timeout     Max time (in millis) we wait for Telit to initiate
 *                      communication.
 *  @param  baudDelay   Max wait between data received.
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

        #ifdef DEBUG
        debugPort->write("Received Data: \r");
        debugPort->write(data);
        debugPort->write("\r");
        #endif
    } else {
        data = NULL;
        dataSize = 0;

        #ifdef DEBUG
        debugPort->write("Receive timed out.\r");
        #endif
    }
    free(receiveBuf);

    return timedOut ? false : true;
}

// TODO: test
/** Finds substring in the response data from Telit, and stores a pointer
 *  to the start of the substring in the field parsedData. If no matching
 *  substring is found, NULL is stored instead.
 *
 *  @param  stringToFind    String of interest.
 *  @return bool            True if substring is found.
 */
bool LTEBase::parseFind(const char* stringToFind) {
    parsedData = strstr(data, stringToFind);
    if (parsedData != NULL) {
        return true;
    } else {
        return false;
    }
}

// TODO: test
/** Finds substring in the response data from Telit, and stores a pointer
 *  to the start of the substring in the field parsedData. If no matching
 *  substring is found, NULL is stored instead.
 *
 *  @param  stringToFind    String of interest.
 *  @return bool            True if substring is found.
 */
bool LTEBase::getCommandOK(const char* command) {
    if (!sendATCommand(command, 2000)) {
        return false;
    }
    if (parseFind("\r\nOK\r\n")) {
        return true;
    } else {
        return false;
    }
}

// TODO: check
/** Prints to debug interface the manufacturer ID, model ID, revision ID,
 *  product serial number ID, and internal mobile subscriber identity.
 *
 *  @return void
 */
void LTEBase::printRegistration() {
    #ifdef DEBUG
    debugPort->write("Printing registration information ...\r");
    #endif

    if (sendATCommand("AT+CGMI") && parseFind("OK")) debugPort->write(data);
    if (sendATCommand("AT+CGMM") && parseFind("OK")) debugPort->write(data);
    if (sendATCommand("AT+CGMR") && parseFind("OK")) debugPort->write(data);
    if (sendATCommand("AT+CGSN") && parseFind("OK")) debugPort->write(data);
    if (sendATCommand("AT+CIMI") && parseFind("OK")) debugPort->write(data);
}

// TODO: finish
/** Determines if the modem is connected to the cell network.
 *
 *  @return bool    True if modem is connected.
 */
bool LTEBase::isConnected() {
    return false;
}

#endif
