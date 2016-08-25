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
 * This LTEHttp class defines an interface that will let you send basic HTTP
 * requests. For more advanced functionality, consider using the TCP/FTP commands
 * the EVK4 provides (AT#TCP... and AT#FTP...) with the LTEBase class.
 *
 * The EVK4 has 3 profiles where it saves information / configurations about the 
 * remote server. This allows you to easily send / receive information from
 * multiple sources at once - but also requires you to specify which profile is
 * used when making HTTP requests.
 *
 * To make an HTTP request, you must first configure one of the HTTP profiles
 * using the setter functions, then specify the profile in the request. All 
 * functions default to profile 1, if only 1 profile is required. In addition, 
 * requests have the "Connection: close" header field included, so there are no
 * persistent connections. The receive() function can then be used to listen for
 * a response.
 */


#ifndef LTE_LTE_HTTP_H_
#define LTE_LTE_HTTP_H_

#include <string.h>

#include "LTEBase.h"

#define DEFAULT_CONN_ID 1
#define DEFAULT_CID 3
#define RECV_BUF_SIZE 10000

class LTEHttp : public LTEBase {
public:
    LTEHttp(HardwareSerial* telitPort, HardwareSerial* debugPort);
    bool init(uint32_t lte_band);

    // TCP/IP stack
    bool openSocket(char* r_ip, int r_port = 80, int conn_id = DEFAULT_CONN_ID, int packet_size = 300, int inactivity_timeout = 90, int connection_timeout = 600);
    bool socketReady();
    int getSocketStatus();
    bool socketPause();
    int socketWrite(char* str);
    int socketReceive();
    bool closeSocket();

    // Basic HTTP functions
    bool httpGET();
    bool httpPOST();

    // Other utility functions
    void reset();
    bool setAuth(int authentication, char* usr = "", char* pass = "");
    bool setSSL(bool ssl);

private:
    int connectionID;
    int cid;

    char hostIP[40];
    char remoteIP[40];
    int remotePort;
    int authType;
    char username[64];
    char password[64];
    bool ssl;
    int timeout;
    int packetSize;
    int socketStatus;

    char receiveBuf[RECV_BUF_SIZE];
    int recvSize;
};

#endif
