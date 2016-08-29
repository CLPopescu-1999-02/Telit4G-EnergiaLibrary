/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * 4G/LTE Library for Telit LE910SV module and Energia.
 *
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


#ifndef LTE_LTE_BASE_
#define LTE_LTE_BASE_

#include "LTEBase.h"


/** LTE Base class constructor.
 *
 *  @param  tp  Telit Serial port pointer.
 *  @param  dp  Debug Serial port pointer.
 */
LTEBase::LTEBase(HardwareSerial* tp, HardwareSerial* dp) {
    telitPort = tp;
    debugPort = dp;
    memset(data, '\0', BUF_SIZE);
    parsedData = NULL;
    bufferFull = false;
}


// TODO: test
/** Sets up initial settings, and selects frequency band that Telit
 *  uses to communicate.
 *
 *  @param  lte_band    Frequency band used by Telit.
 *  @return bool        True on success.
 */
bool LTEBase::init(uint32_t lte_band) {
    #ifdef DEBUG
    debugPort->write("Initializing ...\r\n");
    #endif

    if (!getCommandOK("ATE0")) return false;  // No command echo
    if (!getCommandOK("ATV1")) return false;  // Verbose response
    if (!getCommandOK("AT+IPR=115200")) return false;  // Baud rate
    if (!getCommandOK("AT+CMEE=2")) return false;  // Verbose error reports
    if (!getCommandOK("AT+CGATT=1")) return false;  // GPRS attach  

    /* If you are using a 2G/3G capable device, you would change
     * The arguments here to include your GSM and UMTS bands. The Telit
     * EVK4 (LE910SV-V2) operates on LTE bands 4 and 13.
     * 
     * The command is in this order: AT#BND=GSM,UMTS,LTE
     * 
     * Refer to the Telit AT-command guide to determine the correct values.
     * For LTE, given LTE Band n, the argument passed in is 2 exp(n - 1). For
     * example, LTE band 13 would need us to pass in 2^(13-1) = 4096.
     * */
    char* band = "AT#BND=0,0,8"; // No GSM/UMTS, LTE Band 4
    if (!getCommandOK(band)) return false;

    return true;
}


/** Sends an AT command to the Telit module.
 *
 *  @param  cmd   String containing AT command to send.
 *  @return bool  False if empty string.
 */
bool LTEBase::sendATCommand(const char* cmd) {
    // Invalid arguments
    if ((cmd == NULL) || (cmd[0] == '\0') || (bufferFull)) return false;

    #ifdef DEBUG
    debugPort->write("Sending AT Command: \"");
    debugPort->write(cmd);
    debugPort->write("\"\r\n");
    #endif

    telitPort->write(cmd);
    telitPort->write("\r\n");

    return true;
}


/** Listens to the serial port for data from the Telit module and captures
 *  it in the char* data field. Returns false if no data is received before
 *  function times out.
 *
 *  @param  timeout     Max time (in millis) we wait for Telit to initiate
 *                      communication.
 *  @param  baudDelay   Max wait between data received.
 *  @return bool
 */
bool LTEBase::receiveData(uint32_t timeout, uint32_t baudDelay) {
    // Invalid inputs
    if ((timeout == 0) || (baudDelay == 0)) return false;

    // Block while waiting for the start of the message
    uint32_t startTime = millis();
    while (!telitPort->available()) {
        if ((millis() - startTime) > timeout) {
            return false;  // Timeout
        }
    }

    memset(data, '\0', BUF_SIZE);
    bufferFull = false;

    uint32_t receivedSize = 0;
    parsedData = NULL;

    // Receive data from serial port
    startTime = millis();
    bool timedOut = false;
    while (!timedOut) {
        if (receivedSize >=BUF_SIZE) {
            bufferFull = true;
            break;
        }

        // Store next byte
        char c = (char) telitPort->read();
        if (!((receivedSize == 0) && ((c == '\0') || (c == '\r') || (c == '\n')))) {
            data[receivedSize] = c;
            receivedSize++;
        }
        startTime = millis();
        
        // Wait for more data
        while (telitPort->available() < 1) {
            if ((millis() - startTime) > baudDelay) {
                timedOut = true;
                bufferFull = false;
                break;
            }
        }
    }

    if (receivedSize < BUF_SIZE) data[receivedSize] = '\0';
    recDataSize = receivedSize;

    #ifdef DEBUG
    debugPort->write("--- Received Data ---\r\n");
    debugPort->write(data);
    debugPort->write("--- End Received Data ---\r\n\r\n");
    #endif

    return true;
}


/** Gets stored data that we read from Telit's Serial port. If no data
 *  exists, then return an empty string.
 *
 *  @return char*   Data received from Telit.
 */
char* LTEBase::getData() {
    return data;
}


/** Returns a pointer to the start of the substring found by parseFind().
 *  If parseFind found no matching string, returns empty string.
 *
 *  @return char*   Substring of interest, or empty string.
 */
char* LTEBase::getParsedData() {
    return parsedData;
}

/** Deletes all stored received data from the internal buffer.
 *
 *  @return void
 */
void LTEBase::clearData() {
    memset(data, '\0', BUF_SIZE);
    parsedData = NULL;
    recDataSize = 0;
}


/** Finds substring stringToFind in the response data from Telit,
 *  and stores the the rest of the response data starting from
 *  AFTER stringToFind in the parsedData field. If no matching
 *  substring is found, empty string is stored instead. Empty
 *  string always returns false.
 *
 *  @param  stringToFind    String of interest.
 *  @return bool            True if substring is found.
 */
bool LTEBase::parseFind(const char* stringToFind) {
    if ((stringToFind == NULL) || 
        (stringToFind[0] == '\0') || (data[0] == '\0'))
        return false;
	
    // TODO:
    char* beginning = strstr(data, stringToFind);
    if (beginning == NULL) return false;
    parsedData = beginning + strlen(stringToFind);
    return true;
}


// TODO: test
/** Sends an AT Command, listens for a response, and looks for an
 *  "OK" code in the response. If an "OK" is received, this function
 *  returns true, otherwise it returns false.
 *
 *  @param  command     String containing AT Command.
 *  @return bool        True if OK is received.
 */
bool LTEBase::getCommandOK(const char* command) {
    if (!sendATCommand(command) || !receiveData(2000)) return false;
    if (parseFind("OK\r\n")) {
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
    debugPort->write("Printing registration information ...\r\n");
    #endif

    if (sendATCommand("AT+CGMI") && receiveData() && parseFind("\r\nOK\r\n")) {
        debugPort->write("Manufacturer Identification: ");
        parseFind("\r\n");
        debugPort->write((uint8_t*) data, recDataSize - strlen(parsedData));
    }
    if (sendATCommand("AT+CGMM") && receiveData() && parseFind("\r\nOK\r\n")) {
        debugPort->write("Model Identification: ");
        parseFind("\r\n");
        debugPort->write((uint8_t*) data, recDataSize - strlen(parsedData));
    }
    if (sendATCommand("AT+CGMR") && receiveData() && parseFind("\r\nOK\r\n")) {
        debugPort->write("Revision Identification: ");
        parseFind("\r\n");
        debugPort->write((uint8_t*) data, recDataSize - strlen(parsedData));
    }
    if (sendATCommand("AT+CGSN") && receiveData() && parseFind("\r\nOK\r\n")) {
        debugPort->write("Product Serial Number Identification: ");
        parseFind("\r\n");
        debugPort->write((uint8_t*) data, recDataSize - strlen(parsedData));
    }
    if (sendATCommand("AT+CIMI") && receiveData() && parseFind("\r\nOK\r\n")) {
        debugPort->write("International Mobile Subscriber Number: ");
        parseFind("\r\n");
        debugPort->write((uint8_t*) data, recDataSize - strlen(parsedData));
    }
}


// TODO: test
/** Determines if the modem is connected to the cell network.
 *
 *  @return bool    True if modem is connected.
 */
bool LTEBase::isConnected() {
    if (getCommandOK("AT+CGATT?") && parseFind("+CGATT: 1"))
		return true;
    return false;
}

#endif
