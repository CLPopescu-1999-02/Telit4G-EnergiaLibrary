/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * 4G/LTE Library for Telit LE910SV module and Energia.
 */


#ifndef LTE_LTE_TCP_
#define LTE_LTE_TCP_

#include <stdio.h>
#include <stdlib.h>

#include "LTE_TCP.h"
#include "LTE_Base.h"


/** LTE TCP class constructor.
 *
 *  @param  tp  Telit Serial port.
 *  @param  dp  Debug Serial port pointer.
 */
LTE_TCP::LTE_TCP(HardwareSerial* tp, HardwareSerial* dp)
                    : LTE_Base::LTE_Base(tp, dp) {
    reset();
}

/** Initializes access point and internet settings.
 *
 *  @param  lte_band  4G band to use.
 *  @param  apn       Access point name.
 *  @return bool      True on success.
 */
bool LTE_TCP::init(uint32_t lte_band, char* apn) {
    if (!LTE_Base::init(lte_band))
        return false;
    char cmd[40];
    sprintf(cmd, "AT#SGACT=%d,0", DEFAULT_CID);
    sendATCommand(cmd);
    receiveData(5000, 100);

    sprintf(cmd, "AT+CGDCONT=%d,\"IP\",%s,\"\",0,0", DEFAULT_CID, DEFAULT_APN);
    if (!sendATCommand(cmd) || !receiveData(2000,500) || !parseFind("OK"))
        return false;

    sprintf(cmd, "AT#SGACT=%d,1", DEFAULT_CID);
    sendATCommand(cmd);
    receiveData(5000, 100);
    if (!parseFind("OK"))
        return false;
    return true;
}

/** Sets up and opens a TCP/IP socket with the given settings. By default,
 *  the socket is opened up on connection ID 1. If you would like to use
 *  multiple sockets at once (to query multiple servers, or to have your
 *  application act as both a server/client), you must open other sockets
 *  on different conenction ID's.
 *
 *  @param  r_ip                Remote IP address (in form "xxx.xxx.xxx.xxx")
 *  @param  r_port              Remote port. Default is 80.
 *  @param  conn_id             TCP socket ID. LE910 has the connection IDs
 *                              (1-6). Default is 1.
 *  @param  pkt_size            Packet size in bytes. Default is 300.
 *  @param  inactivity_timeo    Socket times out after there is no data
 *                              exchange for this period of time (seconds).
 *  @param  conn_timeo          Socket times out if it can't establish a
 *                              connection in this period of time (hundreds of
 *                              milliseconds). Range is 10-1200.
 *  @return bool                True on success.
 */
bool LTE_TCP::socketOpen(char* r_ip, int r_port, int conn_id, int pkt_size,
                         int inactivity_timeo, int conn_timeo) {
    if ((r_ip == NULL) || (r_ip[0] == '\0') || (strlen(r_ip) > 255) ||
        (r_port < 1) || (r_port > 65535) || (conn_id < 1) ||
        (conn_id > 6) || (pkt_size <= 0) || (pkt_size > 1500) ||
        (inactivity_timeo < 0) || (inactivity_timeo > 65535) ||
        (conn_timeo < 10) || (conn_timeo > 1200))
        return false;

    strncpy(remoteIP, r_ip, strlen(r_ip));
    remotePort = r_port;

    if (!gprsAttach())
        return false;

    // Configures socket for specified socket connection ID, connection ID
    char cmd[320];
    sprintf(cmd, "AT#SCFG=%d,%d,%d,%d,%d,0",
            conn_id, cid, pkt_size, inactivity_timeo, conn_timeo);
    if (!getCommandOK(cmd))
        return false;
    
    // Activate PDP context
    memset(cmd, '\0', 320);
    sprintf(cmd, "AT#SGACT=%d,0", cid);
    getCommandOK(cmd);
    memset(cmd, '\0', 320);
    sprintf(cmd, "AT#SGACT=%d,1", cid);
    if (!sendATCommand(cmd) || !receiveData(2000, 100))
        return false;
    
    // Get self IP from PDP context
    if (!parseFind("#SGACT: "))
        memset(hostIP, '\0', 40);
    else {
        strncpy(hostIP, getParsedData(), strlen(getParsedData()));
        hostIP[strlen(getParsedData())] = '\0';
    }

    // Open socket
    memset(cmd, '\0', 320);
    sprintf(cmd, "AT#SD=%d,0,%d,%s,255,0,1", connectionID, remotePort, r_ip);
    if (!sendATCommand(cmd) || !receiveData(2000, 100) || !parseFind("OK"))
        return false;

    socketStatus = getSocketStatus();
    return true;
}

/** Returns true if socket is connected and ready to transmit data.
 *
 *  @return	bool
 */
bool LTE_TCP::socketReady() {
    int x = getSocketStatus();
    return ((x == 0) || (x == 6) || (x == 7)) ? false : true;
}

/** Queries the state of the TCP/IP socket at the connection ID. Updates
 *  the private variable.
 *  Socket statuses:
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
int LTE_TCP::getSocketStatus() {
    getCommandOK("AT#SS");
    char match[8];
    sprintf(match, "#SS: %d,", connectionID);
    if (parseFind(match)) {
        socketStatus = (getParsedData())[0] - '0';
        return socketStatus;
    }
    else return -1;
}

/** Wrapper function for writing data to the modem's serial port. Makes use
 *  of the AT#SSEND command.
 *
 *  @param  str     String to write.
 *  @return	int     Number of bytes written. -1 on error.
 */
int LTE_TCP::socketWrite(char* str) {
    if (!socketReady())     // Socket not available
        return -1;
    
    char cmd[12];
    sprintf(cmd, "AT#SSEND=%d", connectionID);
    sendATCommand(cmd);
    receiveData(2000, 100);
    if (!parseFind(">"))    // Timeout, AT#SSEND did not have expected response
        return -1;

    telitPort->write(str);
    telitPort->write((char) 26);    // End AT#SSEND
    receiveData(500, 100);
    
    if (parseFind("OK"))
        return strlen(str);
    else return -1;
}

/** Attempts to receive data from the TCP socket and store it in the receive
 *  buffer. If at any point there is an error, -1 is returned, and partially
 *  received data is saved. This function makes use of the AT#SRECV command.
 *
 *  TODO: There is a bug where if the buffer size of the LTE_Base class is much
 *  smaller than the AT#SRECV buffer, we lose some amount of data. Workaround
 *  is to have LTE_Base have a buffer larger than 1500 bytes (max bytes
 *  AT#SRECV can read at once).
 * 
 *  @return int     Number of bytes received. -1 on error.
 */
int LTE_TCP::socketReceive() {
    if (socketStatus == 0)
        return -1;

    if (receiveBuf != NULL) {
        receiveBuf = NULL;
    }
    
    int currentBufSize = MAX_SRECV_SIZE;
    receiveBuf = (char*) malloc(currentBufSize * sizeof(char));

    int totalBytesReceived = 0;
    char cmd[16];
    sprintf(cmd, "AT#SRECV=%d,%d", connectionID, MAX_SRECV_SIZE);
    char tofind[16];
    sprintf(tofind, "#SRECV: %d,", connectionID);

    // Keep sending AT#SRECV requests until all packets read
    while (true) {
        if (!sendATCommand(cmd) || !receiveData(2000, 100) ||
            !parseFind(tofind))
            break;

        int recPacketSize = (atoi(strsep(&parsedData, "\r\n")));
        int packetBytesLeft = recPacketSize;
        parsedData[-1] = '\r';
        parsedData += 1;    // points to char after "\n" 

        // Reallocate and leave room for terminating null byte
        if (totalBytesReceived + recPacketSize >= currentBufSize - 1) {
            realloc(receiveBuf, currentBufSize*2);
        }

        // In case the internal LTE_Base buffer is smaller than the TCP receive
        // buffer, we need to ask the Base to read multiple times to process
        // the entire AT#SRECV response.
        if ((BASE_BUF_SIZE - (parsedData - data)) >= recPacketSize) {
            memcpy(receiveBuf + totalBytesReceived, parsedData, recPacketSize);
            totalBytesReceived += recPacketSize;
            packetBytesLeft -= recPacketSize;
        }
        else {
            memcpy(receiveBuf + totalBytesReceived, parsedData,
                   (BASE_BUF_SIZE - (parsedData - data)));
            totalBytesReceived += (BASE_BUF_SIZE - (parsedData - data));
            packetBytesLeft -= (BASE_BUF_SIZE - (parsedData - data));
            
            while (packetBytesLeft > 0) {
                if (!receiveData(500, 100)) {
                    return -1;
                }
                
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
bool LTE_TCP::socketClose() {
    char cmd[20];
    sprintf(cmd, "AT#SH=%d", connectionID);
    getCommandOK(cmd);
    memset(cmd, '\0', 20);
    sprintf(cmd, "AT#SGACT=%d,0", cid);
    getCommandOK(cmd);
    if (getSocketStatus() == 0)
        return true;
    return false;
}

/** Resets private variables to their default values.
 *
 *  @return void
 */
void LTE_TCP::reset() {
    connectionID = DEFAULT_CONN_ID;
    cid = DEFAULT_CID;
    memset(hostIP, '\0', 40);
    memset(remoteIP, '\0', 256);
    remotePort = 80;
    socketStatus = 0;
    
    if (receiveBuf != NULL) {
        free(receiveBuf);
        receiveBuf = NULL;
    }
    recvSize = 0;
}

/** Connects to the GPRS network. If already connected, returns true.
 * 
 *  @return bool    True on success
 */
bool LTE_TCP::gprsAttach() {
    getCommandOK("AT+CGATT?");
    if (!parseFind("1")) {
        return getCommandOK("AT+CGATT=1");
    }
    return true;
}

/** Clone of the function LTEBase::parseFind(), but for the LTE_TCP buf.
 *  Finds first instance of substring "stringToFind" in the response data from
 *  the socket, and returns the rest of the data AFTER the substring
 *  "stringToFind". If no matching substring is found, empty string is stored
 *  instead. If "stringToFind" is empty string, function returns false.
 *
 *  @param  stringToFind    String of interest.
 *  @return char*           Substr of receiveBuf, starting after stringToFind
 */
char* LTE_TCP::socketParseFind(const char* stringToFind) {
    if ((stringToFind == NULL) || (stringToFind[0] == '\0') ||
        (receiveBuf == NULL) || (receiveBuf[0] == '\0'))
        return false;
  
    char* beginning = strstr(receiveBuf, stringToFind);
    if (beginning == NULL) return NULL;
    return beginning + strlen(stringToFind);
}

#endif
