/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * Serial AT Command Library for Telit LE910SV module and Energia.
 */


#ifndef LTE_LTE_BASE_
#define LTE_LTE_BASE_

#include <math.h>
#include <stdio.h>
#include "LTE_Base.h"


/** LTE Base class constructor.
 *
 *  @param  tp  Telit Serial port pointer.
 *  @param  dp  Debug Serial port pointer.
 */
LTE_Base::LTE_Base(HardwareSerial* tp, HardwareSerial* dp) {
    #ifdef DEBUG
    debugPort->write(">> Constructing LTE_Base object ...\r\n");
    #endif

    telitPort = tp;
    debugPort = dp;
    memset(data, '\0', BASE_BUF_SIZE);
    parsedData = NULL;
    bufferFull = false;
}

/** Sets up initial settings, and selects frequency band that Telit
 *  uses to communicate.
 *
 *  @param  lte_band    Frequency band used by Telit.
 *  @return bool        True on success.
 */
bool LTE_Base::init(uint32_t lte_band) {
    #ifdef DEBUG
    debugPort->write(">> Initializing LTE_Base ...\r\n");
    #endif

    if (!getCommandOK("ATE0"))          // No command echo
        return false;
    if (!getCommandOK("ATV1"))          // Verbose response from modem
        return false;
    if (!getCommandOK("AT+IPR=115200")) // Baud rate for Serial
        return false;
    if (!getCommandOK("AT+CMEE=2"))     // Verbose error reports
        return false;
    if (!getCommandOK("AT+CGATT=1"))    // GPRS network attached
        return false;

    /* If you are using a 2G/3G capable device, you would change
     * The arguments here to include your GSM and UMTS bands. The Telit
     * EVK4 (LE910SV-V2) operates on LTE bands 4 and 13.
     * 
     * The command is in this order: AT#BND=GSM,UMTS,LTE
     * 
     * Refer to the Telit AT-command guide to determine the correct values.
     * For LTE, given LTE Band n, the argument passed in is 2 exp(n - 1).
	 * For example, LTE band 13 would need us to pass in 2^(13-1) = 4096.
     */
    long double b = pow(2, lte_band-1);

    char band[20];
    sprintf(band, "AT#BND=0,0,%d", b);	// No GSM/UMTS, only LTE Band
    if (!sendATCommand(band) || !receiveData(2000) || !parseFind("OK")) {
        #ifdef DEBUG
        debugPort->write(">> Setting LTE Band failed\r\n");
        #endif
    }
    
    return true;
}

/** Sends an AT command to the Telit module.
 *
 *  @param  cmd   String containing AT command to send.
 *  @return bool  True on success.
 */
bool LTE_Base::sendATCommand(const char* cmd) {
    if ((cmd == NULL) || (cmd[0] == '\0') || (bufferFull)) return false;

    #ifdef DEBUG
    debugPort->write(">> Sending AT Command: \"");
    debugPort->write(cmd);
    debugPort->write("\"\r\n");
    #endif

    telitPort->write(cmd);
    telitPort->write("\r\n");

    return true;
}

/** Listens on the serial port for data from the Telit module and captures
 *  it in the data[] buffer. Returns false if no data is received before
 *  function times out.
 *
 *  @param  timeout     Max wait time (in millis) for modem to initiate
 *                      communication.
 *  @param  baudDelay   Max wait time between bytes received.
 *  @return bool
 */
bool LTE_Base::receiveData(uint32_t timeout, uint32_t baudDelay) {
    if ((timeout == 0) || (baudDelay == 0)) {
		#ifdef DEBUG
		debugPort->write(">> LTE_Base receiveData failed.\r\n");
		#endif
		return false;
	}

    // Block while waiting for the start of the message
    uint32_t startTime = millis();
    while (!telitPort->available()) {
        if ((millis() - startTime) > timeout) {
			#ifdef DEBUG
			debugPort->write(">> LTE_Base receiveData timed out.\r\n");
			#endif
            return false;  // Timeout
        }
    }

    memset(data, '\0', BASE_BUF_SIZE);
    bufferFull = false;

    uint32_t receivedSize = 0;
    parsedData = NULL;

    // Receive data from serial port
    startTime = millis();
    bool timedOut = false;
    while (!timedOut) {
        if (receivedSize >= BASE_BUF_SIZE) {
            bufferFull = true;
			#ifdef DEBUG
			debugPort->write(">> LTE_Base receiveData buffer full.\r\n");
			#endif
            break;
        }

        // Store next byte
        // Ignore initial whitespace
        char c = (char) telitPort->read();
        if ((receivedSize != 0) || (c != '\0') &&
		    (c != '\r') && (c != '\n')) {
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

    if (receivedSize < BASE_BUF_SIZE) data[receivedSize] = '\0';
    recDataSize = receivedSize;

    #ifdef DEBUG
    debugPort->write(">> LTE_Base --- Received Data ---\r\n");
    debugPort->write(data);
    debugPort->write(">> LTE_Base --- End Received Data ---\r\n");
    #endif

    return true;
}

/** Retrieves stored data we received previously. If no data exists, return
 *  empty string.
 *
 *  @return char*
 */
char* LTE_Base::getData() {
    return data;
}

/** Returns a pointer to the start of the substring found by parseFind().
 *  If parseFind found no matching string, returns empty string.
 *
 *  @return char*   Substring of interest, or empty string.
 */
char* LTE_Base::getParsedData() {
    return parsedData;
}

/** Deletes all stored received data from the internal buffer.
 *
 *  @return void
 */
void LTE_Base::clearData() {
    memset(data, '\0', BASE_BUF_SIZE);
    parsedData = NULL;
    recDataSize = 0;
}

/** Finds first instance of substring "stringToFind" in the response data from
 *  the modem, and stores the the rest of the response data starting from
 *  AFTER "stringToFind" in the parsedData field. If no matching substring is
 *  found, empty string is stored instead. If "stringToFind" is empty string,
 *  function returns false.
 *
 *  @param  stringToFind    String of interest.
 *  @return bool            True if substring is found.
 */
bool LTE_Base::parseFind(const char* stringToFind) {
    if ((stringToFind == NULL) || (stringToFind[0] == '\0') ||
        (data[0] == '\0'))
        return false;
	
    char* beginning = strstr(data, stringToFind);
    if (beginning == NULL) {
		#ifdef DEBUG
		debugPort->write(">> LTE_Base parseFind failed for string \"");
		debugPort->write(stringToFind);
		debugPort->write("\".\r\n");
		#endif
		return false;
    }
	parsedData = beginning + strlen(stringToFind);
    return true;
}

/** Sends an AT Command, listens for a response, and looks for an "OK" code
 *  in the response data. If a "OK" is received, this function returns true.
 *
 *  @param  command     String containing AT Command.
 *  @return bool        True if OK is received.
 */
bool LTE_Base::getCommandOK(const char* command) {
    if (!sendATCommand(command) || !receiveData(500, 100)) return false;
    if (parseFind("OK\r\n")) {
		#ifdef DEBUG
		debugPort->write(">> OK found for command \"");
		debugPort->write(command);
		debugPort->write("\"\r\n");
		#endif
        return true;
    } else {
		#ifdef DEBUG
		debugPort->write(">> OK not found for command \"");
		debugPort->write(command);
		debugPort->write("\"\r\n");
		#endif
        return false;
    }
}

/** Prints to debug interface the manufacturer ID, model ID, revision ID,
 *  product serial number ID, and internal mobile subscriber identity.
 *
 *  @return void
 */
void LTE_Base::printRegistration() {
    #ifdef DEBUG
    debugPort->write(">> Printing registration information ...\r\n");
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

/** Determines if the modem is connected to the GPRS network.
 *
 *  @return bool    True if modem is connected.
 */
bool LTE_Base::isConnected() {
    if (getCommandOK("AT+CGATT?") && parseFind("+CGATT: 1")) {
		#ifdef DEBUG
		debugPort->write(">> GPRS is attached.");
		#endif
		return true;
    }
	#ifdef DEBUG
	debugPort->write(">> GPRS is not attached.");
	#endif
	return false;
}

#endif

