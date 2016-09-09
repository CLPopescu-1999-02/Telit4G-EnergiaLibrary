/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * 4G/LTE Library for Telit LE910SV module and Energia.
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
 * This LTE_TCP class defines an interface that will let you send packets over
 * a TCP/GPRS connection. This can be used for various protocols, including
 * HTTP, MQTT, etc. However, the Telit LE910 has additional, more specific AT
 * commands for HTTP/SMTP/FTP/etc, and if you are using one of these protocols
 * exclusively, it is worth looking into using the specific AT commands rather
 * than the TCP connection.
 */


#ifndef LTE_LTE_TCP_H_
#define LTE_LTE_TCP_H_

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "LTE_Base.h"

#define DEFAULT_CONN_ID 1
#define DEFAULT_CID     3              // PDP context ID.
#define RECV_BUF_SIZE   10000
#define DEFAULT_APN     "vzwinternet"  // Change to your APN.
#define MAX_SRECV_SIZE  1500           // AT#SRECV size. Should not be changed.

class LTE_TCP : public LTE_Base {
public:
    LTE_TCP(HardwareSerial* telitPort, HardwareSerial* debugPort = NULL);
    virtual bool init(uint32_t lte_band, char* apn = DEFAULT_APN);

    char* receivedData() {
        return (receiveBuf == NULL) ? (char*) "" : receiveBuf;
    }

    // TCP/IP stack
    bool socketOpen(char* r_ip, int r_port = 80, int conn_id = DEFAULT_CONN_ID,
                    int packet_size = 300, int inactivity_timeout = 180,
                    int connection_timeout = 600);
    bool socketReady();
    int getSocketStatus();
    int socketWrite(char* str);
    int socketReceive();
    bool socketClose();
    char* getReceivedData() { return (receiveBuf == NULL) ? (char*) "" : receiveBuf; };

    // Other utility functions
    void reset();
    bool gprsAttach();
    char* socketParseFind(const char* stringToFind);

private:
    int connectionID;   // Socket ID. Numbers 1-6
    int cid;            // PDP context ID. For LE910, numbers 1-3

    char hostIP[40];
    char remoteIP[256]; // String containing remote IP or URL to be solved
                        // by DNS query. Ex: "123.456.789" or "www.google.com"

    int remotePort;     // Remote TCP port
    int socketStatus;   // See getSocketStatus()

    char* receiveBuf;
    int recvSize;
};

#endif
