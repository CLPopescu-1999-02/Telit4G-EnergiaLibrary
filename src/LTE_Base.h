/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * Serial AT Command Library for Telit LE910SV module and Energia.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 *
 * This LTE_Base class defines a basic interface to communicate with a Telit
 * LE910 modem via UART. It abstracts some basic AT commands (commands that
 * the modem understands) into functions, but leaves a lot of room for
 * customization.
 *
 * The LTE_Base constructor requires the user to provide a Serial object where
 * the LaunchPad and EVK4 can send/receive AT commands. The init() function
 * then sets basic rules about the communication (for example, verbose error
 * reports).
 *
 * The functions sendATCommand() and getCommandOK() allow you to write AT 
 * commands to the Serial port and listen for a response. sendATCommand()
 * listens for any response, while getCommandOK() requires the modem to
 * respond with an "OK". The response can be processed using the parseFind()
 * function, or can be directly accessed with parseData() if you want to apply
 * your own processing.
 *
 * The very basic commands printRegistration() and isConnected() provided allow
 * you to verify the connection between both the EVK4 and the LaunchPad, as
 * well as with the network.
 */


#ifndef LTE_LTE_BASE_H_
#define LTE_LTE_BASE_H_


// Uncomment this to enable debugging messages
//#define DEBUG

#include <Energia.h>

#include <stdlib.h>
#include <string.h>

#define BASE_BUF_SIZE 2000


class LTE_Base {
public:
    // Basic setup
    LTE_Base(HardwareSerial* telitPort, HardwareSerial* debugPort = NULL);
    virtual ~LTE_Base() {};
    virtual bool init(uint32_t lte_band);

    // Telit communication
    // Basic functions
    virtual char* getData();
    virtual char* getParsedData();
    virtual void clearData();
    virtual bool sendATCommand(const char*);
    virtual bool receiveData(uint32_t timeout = 2000,
                             uint32_t baudDelay = 60);

    // More abstracted functions
    virtual bool parseFind(const char*);    // Search for substring in data
    virtual bool getCommandOK(const char*); // Send command, verify OK response

    // Basic commands for Telit module
    virtual void printRegistration();   // Prints serial numbers
    virtual bool isConnected();         // Connection status

protected:
    HardwareSerial* telitPort;  // Telit serial interface
    HardwareSerial* debugPort;  // Pointer so it can default to null
    char data[BASE_BUF_SIZE];   // Response data from Telit
    uint32_t recDataSize;       // Size of response data from Telit
    char* parsedData;           // Parsed response data
    bool bufferFull;            // Internal data[] buffer full
};

#endif
