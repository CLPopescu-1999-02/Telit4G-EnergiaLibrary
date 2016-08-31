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


#ifndef LTE_LTE_HTTP_
#define LTE_LTE_HTTP_

#include <stdio.h>
#include <stdlib.h>

#include "LTEHttp.h"
#include "LTEBase.h"


/** LTE Http class constructor. Sets private variables to their
 *  default/initial vaues.
 *
 *  @param  tp  Telit Serial port.
 *  @param  dp  Debug Serial port pointer.
 */
LTEHttp::LTEHttp(HardwareSerial* tp, HardwareSerial* dp) : LTEBase::LTEBase(tp, dp) {
    reset();
}

/** Initializes access point settings to connect to internet.
 *
 *  @param  lte_band  4G band to use.
 *  @param  apn       Access point name.
 *  @return bool      True on Success
 */
bool LTEHttp::init(uint32_t lte_band, char* apn) {
    if (!LTEBase::init(lte_band)) return false;
    char cmd[40];
    sprintf(cmd, "AT#SGACT=%d,0", DEFAULT_CID);
    sendATCommand(cmd);
    receiveData(2000,1000);

    // Change this line
    sprintf(cmd, "AT+CGDCONT=%d,\"IP\",\"vzwinternet\",\"\",0,0", DEFAULT_CID);
    if (!getCommandOK(cmd)) return false;
    
    sprintf(cmd, "AT#SGACT=%d,1", DEFAULT_CID);
    sendATCommand(cmd);
    receiveData(2000,1000);
    if (!parseFind("OK")) return false;
    
    return true;
}


/** Sets up and opens a TCP/IP socket with the given settings. By default,
 *  the socket is opened up on connection ID 1. If you would like to use
 *  multiple sockets at once (to query multiple servers, or to have your
 *  application act as both a server/client), you must open other sockets
 *  on different conenction ID's.
 *
 *  @param  r_ip                Remote IP address (in form "xxx.xxx.xxx.xxx")
 *  @param  r_port              Remote port. Default 80.
 *  @param  conn_id             Specific ID for the TCP/IP socket to use. Telit
 *                              EVK4 has the connection IDs (1-6). Default is 1.
 *  @param  pkt_size            Packet size in bytes. Default is 300.
 *  @param  inactivity_timeo    Socket times out after there is no data exchange
 *                              for this period of time. Measured in seconds.
 *  @param  conn_timeo          Socket times out if it can't establish a connection
 *                              in this period of time. Measured in hundreds of
 *                              milliseconds. Range is 10-1200.
 *  @return bool                True on success.
 */
bool LTEHttp::socketOpen(char* r_ip, int r_port, int conn_id, int pkt_size,
                         int inactivity_timeo, int conn_timeo) {
    // invalid 
    if ((r_ip == NULL) || (r_ip[0] == '\0') || (r_port < 1) ||
        (r_port > 65535) || (conn_id < 1) || (conn_id > 6) ||
        (pkt_size <= 0) || (pkt_size > 1500) ||
        (inactivity_timeo < 0) || (inactivity_timeo > 65535) ||
        (conn_timeo < 10) || (conn_timeo > 1200)) return false;

    strncpy(remoteIP, r_ip, strlen(r_ip));
    remotePort = r_port;

    if (!gprsAttach()) return false;

    // Choose which connection you would like to use with the CID
    // Sets socket configuratiion for the specific ID
    char cmd[64];
    sprintf(cmd, "AT#SCFG=%d,%d,%d,%d,%d,0",
            conn_id, cid, pkt_size, inactivity_timeo, conn_timeo);
    if (!getCommandOK(cmd)) return false;
    
    // Activate PDP context
    memset(cmd, '\0', 64);
    sprintf(cmd, "AT#SGACT=%d,0", cid);
    getCommandOK(cmd);
    memset(cmd, '\0', 64);
    sprintf(cmd, "AT#SGACT=%d,1", cid);
    if (!sendATCommand(cmd) || !receiveData(10000,2000)) return false;
    
    // Get self IP from PDP context
    if (!parseFind("#SGACT: ")) memset(hostIP, '\0', 40);
    else {
        strncpy(hostIP, getParsedData(), strlen(getParsedData()));
        hostIP[strlen(getParsedData())] = '\0';
    }

    // Open socket
    memset(cmd, '\0', 64);
    sprintf(cmd, "AT#SD=%d,0,%d,%s,255,0,1", connectionID, remotePort, r_ip);
    if (!sendATCommand(cmd) || !receiveData(10000,1000) || !parseFind("OK")) return false;

    socketStatus = getSocketStatus();
    return true;
}


/** Checks if socket is connected and ready to transmit data.
 *
 *  @return	bool	True if socket is ready to transmit data.
 */
bool LTEHttp::socketReady() {
    int x = getSocketStatus();
    return ((x == 0) || (x == 6) || (x == 7)) ? false : true;
}


/** Queries the state of the TCP/IP socket at the connection ID. Updates
 *  the private variable.
 *  	0: Socket closed
 *  	1: Socket with active data transfer connection
 *  	2: Socket suspended
 *  	3: Socket suspended with pending data
 *  	4: Socket listening
 *  	5: Socket with incoming connection, waiting for
 *  	   user accept or shutdown command
 *  	6: Socket resolving DNS.
 *  	7: Socket connecting.
 *
 *  @return	int		Current socket state. -1 for error.
 */
int LTEHttp::getSocketStatus() {
    getCommandOK("AT#SS");
    char match[8];
    sprintf(match, "#SS: %d,", connectionID);
    if (parseFind(match)) {
        socketStatus = (getParsedData())[0] - '0';
        return socketStatus;
    }
    else return -1;
}


/** Wrapper function for writing to the Telit serial port.
 *
 *  @param  str     String to write.
 *  @return	int     Number of bytes written. -1 is error.
 */
int LTEHttp::socketWrite(char* str) {
    if (!socketReady()) return -1;  // Socket not available
    
    char cmd[16];
    sprintf(cmd, "AT#SSEND=%d", connectionID);
    sendATCommand(cmd);
    receiveData(5000,500);
    if (!parseFind(">"))
        return -1;  // Timeout, AT#SSEND did not have expected response

    telitPort->write(str);
    telitPort->write((char) 26);  // End AT#SSEND
    receiveData(500);
    
    if (parseFind("OK")) return strlen(str);
    else return -1;
}


/** TODO: Right now there exists a problem where, if the base buffer is much smaller than the MAX_SRECV_SIZE, then we lose data (we cannot receive from the Telit Serial quickly enough). Go debug this later if you have time
 * 
 */
int LTEHttp::socketReceive() {
    if (receiveBuf != NULL) {
        free(receiveBuf);
        receiveBuf = NULL;
    }
    
    int currentBufSize = MAX_SRECV_SIZE;
    receiveBuf = (char*) malloc(currentBufSize * sizeof(char));

    int totalBytesReceived = 0;
    char cmd[16];
    sprintf(cmd, "AT#SRECV=%d,%d", connectionID, MAX_SRECV_SIZE);
    char tofind[16];
    sprintf(tofind, "#SRECV: %d,", connectionID);

    // Loop AT#SRECV until all http packets read
    while (true) {
        if (!sendATCommand(cmd) || !receiveData(10000,500) || !parseFind(tofind)) break;

        int recPacketSize = (atoi(strsep(&parsedData, "\r\n")));
        int packetBytesLeft = recPacketSize;
        parsedData[-1] = '\r';
        parsedData += 1;  // points to char after "\n" 

        // realloc more space if needed; leave room for terminating null byte
        if (totalBytesReceived + recPacketSize >= currentBufSize - 1) {
            realloc(receiveBuf, currentBufSize*2);
        }

        // In case the internal LTE_Base buffer is smaller than our HTTP Packet receive size,
        // we need to ask the Base to read multiple times to process the entire AT#SRECV response
        if ((BASE_BUF_SIZE - (parsedData - data)) >= recPacketSize) {
            memcpy(receiveBuf + totalBytesReceived, parsedData, recPacketSize);
            totalBytesReceived += recPacketSize;
            packetBytesLeft -= recPacketSize;
        }
        else {
            memcpy(receiveBuf + totalBytesReceived, parsedData, (BASE_BUF_SIZE - (parsedData - data)));
            totalBytesReceived += (BASE_BUF_SIZE - (parsedData - data));
            packetBytesLeft -= (BASE_BUF_SIZE - (parsedData - data));

            while (packetBytesLeft > 0) {
                if (!receiveData(500,100)) return -1;
                
                if (packetBytesLeft > BASE_BUF_SIZE) {
                    memcpy(receiveBuf + totalBytesReceived, data, recDataSize);
                    totalBytesReceived += recDataSize;
                    packetBytesLeft -= recDataSize;
                }
                else {
                    memcpy(receiveBuf + totalBytesReceived, data, packetBytesLeft);
                    totalBytesReceived += packetBytesLeft;
                    packetBytesLeft = 0;
                }
            }
        }
    }
    receiveBuf[totalBytesReceived] = '\0';

    return totalBytesReceived;
}


/** Closes socket and closes PDP context.
 *
 *  @return	bool	True on success.
 */
bool LTEHttp::socketClose() {
    char cmd[40];
    sprintf(cmd, "AT#SH=%d", connectionID);
    getCommandOK(cmd);
    memset(cmd, '\0', 40);
    sprintf(cmd, "AT#SGACT=%d,0", cid);
    getCommandOK(cmd);
    if (getSocketStatus() == 0) return true;
    return false;
}

/** Connects to the GPRS network. If already connected,
 *  returns true.
 * 
 *  @return bool  True on success
 */
bool LTEHttp::gprsAttach() {
    getCommandOK("AT+CGATT?");
    if (!parseFind("1")) {
        sendATCommand("AT+CGATT=1");
        receiveData(2000,1000);
        return parseFind("OK");
    }
    return true;
}

/** Resets private variables to their default values.
 *
 *  @return void
 */
void LTEHttp::reset() {
    connectionID = DEFAULT_CONN_ID;
    cid = DEFAULT_CID;
    memset(hostIP, '\0', 40);
    memset(remoteIP, '\0', 40);
    remotePort = 80;
    authType = 0;
    memset(username, '\0', 64);
    memset(password, '\0', 64);
    ssl = false;
    timeout = 90;
    packetSize = 300;
    socketStatus = 0;
    
    if (receiveBuf != NULL) {
        free(receiveBuf);
        receiveBuf = NULL;
    }
    recvSize = 0;
}

#endif
