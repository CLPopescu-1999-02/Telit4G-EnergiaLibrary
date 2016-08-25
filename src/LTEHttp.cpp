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
LTEHttp::LTEHttp(HardwareSerial* tp, HardwareSerial* dp) : LTEBase(tp, dp) {
    reset();
}

/** Selects frequency band used by Telit to communicate
 *
 *  @param  lte_band    Frequency band used by Telit.
 *  @return bool        True on success.
 */
bool LTEHttp::init(uint32_t lte_band) {
    LTEBase::init(lte_band);
    return isConnected();
}


/** Sets up and opens a TCP/IP socket with the given settings. By default,
 *  the socket is opened up on connection ID 1. If you would like to use
 *  multiple sockets at once (to query multiple servers, or to have your
 *  application act as both a server/client), you must open the sockets on
 *  different conenction ID's.
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
 *  @return                     Boolean on success.
 */
bool LTEHttp::openSocket(char* r_ip, int r_port, int conn_id, int pkt_size,
                         int inactivity_timeo, int conn_timeo) {
    // invalid 
    if ((r_ip == NULL) || (r_ip[0] == '\0') || (r_port < 1) ||
        (r_port > 65535) || (conn_id < 1) || (conn_id > 6) ||
        (pkt_size <= 0) || (pkt_size > 1500) ||
        (inactivity_timeo < 0) || (inactivity_timeo > 65535) ||
        (conn_timeo < 10) || (conn_timeo > 1200)) return false;

    strncpy(remoteIP, r_ip, strlen(r_ip)+1);
    remotePort = r_port;

    // TODO: check/make sure steps are correct
    // Set up context number properly
    // +CGDCONT to look up context (figure out which is activated already)
    // #SCFG to set socket settings in specific context to the ones specified
    // #SGACT=context,status to activate
    // look at the return to figure out your own ip
    // AT#SGACTAUTH for authentication
    // AT#SD to dial

    // Choose which connection you would like to use with the CID
    // Sets socket configuratiion for the specific ID
    char cmd[64];
    sprintf(cmd, "AT#SCFG=%d,%d,%d,%d,%d,0",
            conn_id, cid, pkt_size, inactivity_timeo, conn_timeo);
    if (!getCommandOK(cmd)) return false;

    // Activate PDP context
    memset(cmd, '\0', 64);
    sprintf(cmd, "AT#SGACT=%d,0", cid);
    sendATCommand(cmd);

    memset(cmd, '\0', 64);
    sprintf(cmd, "AT#SGACT=%d,1", cid);
    if (!getCommandOK(cmd) || !parseFind("#SGACT: ")) return false;
    char* temp = getParsedData();
    if (strstr(temp, ",") == NULL) {
        strncpy(hostIP, temp, strlen(temp)+1);
    }
    else strncpy(hostIP, temp, strstr(temp, ",") - temp);

    // Open socket
    memset(cmd, '\0', 64);
    sprintf(cmd, "AT#SD=%d,%d,%d,255,0,0", cid, remotePort, remoteIP);
    if (!getCommandOK(cmd)) return false;

    // TODO: check to see if socket actually opened
    // AT#SS
    // should wait for a bit before you return this
    // 4 seconds?
    return getSocketStatus() == 2 ? true : false;

    return true;
}

// converted
bool LTEHttp::socketReady() {
    return getSocketStatus() == 1 ? true : false;
}

// converted
int LTEHttp::getSocketStatus() {
    getCommandOK("AT#SS");
    char match[8];
    sprintf(match, "#SS: %d", connectionID);
    parseFind(match);
    socketStatus = getParsedData()[0] - '0';
    return socketStatus;
}

bool LTEHttp::socketPause() {
    if ((socketStatus == 1) || (socketStatus == 4)) {
        // write +++
        // TODO: finish
        
        getSocketStatus();
        
        return true;
    }
    else return false;
}

int LTEHttp::socketWrite(char* str) {
    // TODO: also figure this out
    if (!socketReady()) return -1;
    return telitPort->write(str);
}

int LTEHttp::socketReceive() {
    // figure this out
    // TODO:
}

bool LTEHttp::closeSocket() {
    if (getSocketStatus() != 0) getCommandOK("AT#SH");
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
    timeout = 120;
    packetSize = 300;
    socketStatus = getSocketStatus();

    memset(receiveBuf, '\0', RECV_BUF_SIZE);
    recvSize = 0;
}

#endif
