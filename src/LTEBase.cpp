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
	data = "";
	parsedData = "";
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

    if (!sendATCommand("ATE0", 500, 2000)) return false;  // No command echo
    if (!getCommandOK("ATV1")) return false;  // Verbose response
    if (!getCommandOK("AT+IPR=115200")) return false;  // Baud rate
    if (!getCommandOK("AT+CMEE=2")) return false;  // Verbose error reports
    if (!getCommandOK("AT&K0")) return false;  // No flow control

    if (!getCommandOK("AT+FCLASS=0")) return false;  // Data calls

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
    std::string band = "AT#BND=0,0,8"; // No GSM/UMTS, LTE Band 4
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
bool LTEBase::sendATCommand(const std::string cmd, uint32_t timeout,
                            uint32_t baudDelay) {
    // Invalid arguments
    if (timeout == 0 || baudDelay == 0) return false;
    if (cmd == "") return false;

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

    // Block while waiting for the start of the message
    uint32_t startTime = millis();
    while (telitPort->available() < 1) {
        if ((millis() - startTime) > timeout) {
            return false;  /* Timeout */
        }
    }
    
    std::string receiveBuf = "";
    uint32_t receivedSize = 0;

    // Receive data from serial port
    startTime = millis();
    bool timedOut = false;
    while (!timedOut) {
        // Store next byte
        receiveBuf += (char) telitPort->read();
        receivedSize = 0;
        startTime = millis();

        // Wait for more data
        while (telitPort->available() < 1) {
            if ((millis() - startTime) > baudDelay) {
                timedOut = true;
                break;
            }
        }
    }

    data = receiveBuf;
    recDataSize = receivedSize;
    parsedData = "";

    #ifdef DEBUG
    debugPort->write("Received Data: \r\n");
    debugPort->write(data);
    debugPort->write("\r\n");
    #endif

    return true;
}


/** Gets stored data that we read from Telit's Serial port. If no data
 *  exists, then return an empty string.
 *
 *  @return std::string     Data received from Telit.
 */
std::string LTEBase::getData() {
    return data;
}


/** Returns a pointer to the start of the substring found by parseFind().
 *  If parseFind found no matching string, returns empty string.
 *
 *  @return std::string     Substring of interest, or empty string.
 */
std::string LTEBase::getParsedData() {
    return parsedData;
}

/** Deletes all stored received data from the internal buffer.
 *
 *  @return void
 */
void LTEBase::clearData() {
    data = "";
    parsedData = "";
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
bool LTEBase::parseFind(const std::string stringToFind) {
    if ((stringToFind == "") || 
		 (stringToFind == "") || (data == ""))
		return false;
	
    int substrPos = data.find(stringToFind);
    if (substrPos == std::string::npos) return false;
    parsedData = data.substr(substrPos + stringToFind.length());
    return true;
}


// TODO: test
/** Sends an AT Command, listens for a response, and looks for an
 *  "OK" code in the response. If an "OK" is received, this function
 *  returns true, otherwise it returns false.
 *
 *  @param  std::string     AT Command.
 *  @return bool            True if OK is received.
 */
bool LTEBase::getCommandOK(const std::string command) {
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

// TODO: test
/** Determines if the modem is connected to the cell network, and returns
 *  a Received Signal Strength Indicator (integer between 0 and 7).
 *  0: -4 to -3 dBm
 *  1: -6 to -5 dBm
 *  2: -8 to -7 dBm
 *  3: -10 to -9 dBm
 *  4: -13 to -11 dBm
 *  5: -15 to -14 dBm
 *  6: -17 to -16 dBm
 *  7: -19 to -18 dBm
 *  -1: Not connected/no signal detected
 *
 *  @return int     Received Signal Strength Indication (between 0 and 7)
 */
int LTEBase::getConnectionStrength() {
    if (!sendATCommand("AT+CSQ") || !parseFind(",")) return false;
    if (parsedData.find("\r\n") != std::string::npos) {
        std::string rsrq = parsedData.substr(0, parsedData.find("\r\n"));
        if (stoi(rsrq) == 99) return false;
        else return stoi(rsrq);
    }
    else return -1;
}

// TODO: test
/** Determines if the modem is connected to the cell network.
 *
 *  @return bool    True if modem is connected.
 */
bool LTEBase::isConnected() {
    if (getConnectionStrength() != -1) return true;
    return false;
}

#endif
