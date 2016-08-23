/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * 4G/LTE Library for Telit LE910SV module and Energia.
 *
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 *
 *
 * This LTEBase class defines a basic interface to communicate with the 
 * Telit EVK4 BoosterPack via UART. It abstracts some basic AT commands (commands
 * the modem uses) into functions, but leaves a lot of room for customization /
 * custom commands.
 *
 * The LTEBase constructor requires the user to provide a Serial object where
 * the LaunchPad and EVK4 can send/receive AT commands. The init() function then
 * sets basic rules about the communication (for example, verbose error reports).
 *
 * The functions sendATCommand() and getCommandOK() allow you to write AT commands
 * to the Serial port and listen for a response. sendATCommand() listens for any 
 * response, while getCommandOK() requires an "OK". The response can be processed
 * using the parseFind() function, or can be directly accessed with parseData() if
 * you want to apply your own processing.
 *
 * The very basic commands printRegistration() and isConnected() provided allow
 * you to verify the connection between both the EVK4 and the LaunchPad, as well
 * as with the network.
 */


#ifndef LTE_LTE_BASE_H_
#define LTE_LTE_BASE_H_


// Uncomment this to enable debugging messages
// #define DEBUG

// Uncomment this to enable unit testing
#define UNIT_TEST


#include <stdlib.h>
#include <string.h>

#ifdef UNIT_TEST
#include "../test/MockEnergia.h"
#else
#include <Energia.h>
#endif

class LTEBase {
public:
    // Basic setup
    LTEBase(HardwareSerial* telitPort, HardwareSerial* debugPort = NULL);
    virtual bool init(uint32_t lte_band);

    // Telit communication
    // Basic functions
    virtual std::string getData();
    virtual std::string getParsedData();
    virtual void clearData();
    virtual bool sendATCommand(const std::string, uint32_t timeout = 10000,
                             uint32_t baudDelay = 60);
    virtual bool receiveData(uint32_t timeout, uint32_t baudDelay);

    // More abstracted functions
    virtual bool parseFind(const std::string);  // Search for substring in data
    bool getCommandOK(const std::string);  // Send command, verify OK response

    // Basic commands for Telit module
    virtual void printRegistration();  // Prints serial numbers
    virtual bool isConnected();  // Connection status
    virtual int getConnectionStrength();  // Connection strength

protected:
    HardwareSerial* telitPort;  // Telit serial interface
    HardwareSerial* debugPort;  // Pointer so it can default to null
    std::string data;  // Response data from Telit
    uint32_t recDataSize;  // Size of response data from Telit
    std::string parsedData;  // Parsed response data
};

#endif
