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

#include "LTEBase.h"


/** LTE Base class constructor.
 *
 *  @param  tp  Telit Serial port pointer.
 *  @param  dp  Debug Serial port pointer.
 */
LTEBase::LTEBase(HardwareSerial* tp, HardwareSerial* dp) {
    telitPort = tp;
    debugPort = dp;
	data = NULL;
	parsedData = NULL;
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

    //TODO: look up AT commands to set frequency bands

    // Set no echo
    if (!sendATCommand("ATE0", 500, 2000)) return false;

    if (!getCommandOK("ATV1")) return false;  // Verbose response
    if (!getCommandOK("AT+IPR=115200")) return false;  // Baud rate
    if (!getCommandOK("AT+CMEE=2")) return false;  // Verbose error reports
    if (!getCommandOK("AT&K0")) return false;  // No flow control

    /* If you are using a 2G/3G capable device, you would change
     * The arguments here to include your GSM and UMTS bands. For the Telit
     * EVK4 (LE910SV-V2), _band can be 4 or 13. */
    char* band = '\0';
    int gsm_band = 0;
    int umts_band = 0;
    if (!getCommandOK(band)) return false;

    return true;
}


/** Sends an AT command to the Telit module, and lets the user know
 *  if there is a response.
 *
 *  @param  cmd   String containing AT command to send.
 *  @return bool  True if there is a response from the module,
 *                false otherwise.
 */
bool LTEBase::sendATCommand(const char* cmd, uint32_t timeout,
                            uint32_t baudDelay) {
    // Invalid arguments
    if (timeout == 0 || baudDelay == 0) return false;
    if (cmd == NULL) return false;

    #ifdef DEBUG
    debugPort->write("Sending AT Command: ");
    debugPort->write(cmd);
    debugPort->write(" ...\r\n");
    #endif

    telitPort->write(cmd);
    telitPort->write("\r\n");

    return receiveData(timeout, baudDelay);
}


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

    // Invalid inputs
    if ((timeout == 0) || (baudDelay == 0)) return false;

    // Initialize receive buffer
    uint32_t dataSize = 20;  /* Initial size (in chars) of buffer used to
                                 store received data (default is 20) */

    char* receiveBuf = (char*) malloc(dataSize * sizeof(char));
    memset((void*) receiveBuf, '\0', dataSize);
    if (receiveBuf == NULL) {  /* No more memory to allocate */
        return false;
    }

    // Block while waiting for the start of the message
    uint32_t startTime = millis();
    while (telitPort->available() < 1) {
        if ((millis() - startTime) > timeout) {
            free(receiveBuf);
            return false;  /* Timeout */
        }
    }
    
    // Receive data from serial port
    uint32_t dataPos = 0;
    startTime = millis();
    bool timedOut = false;
    while (!timedOut) {
        // Expand buffer if it's full
        if (dataPos >= dataSize) {
            dataSize *= 2;
            receiveBuf = (char*) realloc(receiveBuf, dataSize);
			if (receiveBuf == NULL) {
				return false;
				// TODO: possible memory leak if we realloc and no more space
			}
        }

        // Store next byte
        receiveBuf[dataPos] = telitPort->read();
        dataPos++;
        startTime = millis();

        // Wait for more data
        while (telitPort->available() < 1) {
            if ((millis() - startTime) > baudDelay) {
                if (dataPos >= dataSize) {
					receiveBuf = (char*) realloc(receiveBuf, dataSize+1);
					if (receiveBuf == NULL) return false;
						// TODO: possible memory leak here too
				}
                receiveBuf[dataPos] = '\0';  // Null terminate
                dataPos++;
                timedOut = true;
                break;
            }
        }
    }

    // Free previously stored data
    if (data != NULL) {
        free(data);
        data = NULL;
    }

    data = (char*) malloc((dataPos) * sizeof(char));
    memcpy(data, receiveBuf, dataPos);
    recDataSize = dataPos;

    #ifdef DEBUG
    debugPort->write("Received Data: \r\n");
    debugPort->write(data);
    debugPort->write("\r\n");
    #endif

    free(receiveBuf);

    return true;
}


/** Gets stored data that we read from Telit's Serial port. If no data
 *  exists, then return an empty string.
 *
 *  @return char*           Response data, or empty string if data is NULL
 */
char* LTEBase::getData() {
    if (data != NULL)
        return data;
    else {
        return (char*) '\0';
	}
}


/** Returns a pointer to the start of the substring found by parseFind().
 *  If parseFind found no matching string, this contains NULL.
 *
 *  @return char*           Substring of interest, or NULL.
 */
char* LTEBase::getParsedData() {
    if (parsedData != NULL)
        return parsedData;
    else {
        return NULL;
	}
}

/** Deletes all stored received data from the internal buffer.
 *
 *  @return void
 */
void LTEBase::clearData() {
    if (data != NULL) {
        free(data);
        data = NULL;
    }
	parsedData = NULL;
    recDataSize = 0;
}


/** Finds substring in the response data from Telit, and stores a pointer
 *  to the start of the substring in the field parsedData. If no matching
 *  substring is found, NULL is stored instead. Empty string always
 *  returns false.
 *
 *  @param  stringToFind    String of interest.
 *  @return bool            True if substring is found.
 */
bool LTEBase::parseFind(const char* stringToFind) {
    if ((stringToFind == NULL) || 
		 (stringToFind[0] == '\0') || (data == NULL))
		return false;
	
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
    debugPort->write("Printing registration information ...\r\n");
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
