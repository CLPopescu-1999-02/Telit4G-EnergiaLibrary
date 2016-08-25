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

#define DEFAULT_PROFILE 1

class LTEHttp : public LTEBase {
public:
    LTEHttp(HardwareSerial* telitPort, HardwareSerial* debugPort);
    bool init(uint32_t lte_band);

    // Setters
    bool setAddress(int ip1, int ip2, int ip3, int ip4,
                    int profile = DEFAULT_PROFILE);  // Set IP address
    bool setAddress(std::string hostname, int profile = DEFAULT_PROFILE);  // Host name solved with a DNS query
    bool setServerPort(int remote_port, int profile = DEFAULT_PROFILE);
    bool setAuthentication(bool auth_type, std::string usr = "",
                           std::string pass = "", int profile = DEFAULT_PROFILE);
    bool setSSLEnabled(bool ssl, int profile = DEFAULT_PROFILE);
    bool setTimeout(int timeo, int profile = DEFAULT_PROFILE);
    bool setCID(int cid, int profile = DEFAULT_PROFILE);
    bool setPacketSize(int packet_size, int profile = DEFAULT_PROFILE);

    // Getters
    std::string getAddress(int profile = DEFAULT_PROFILE) { return serverIP[profile]; };
    int getServerPort(int profile = DEFAULT_PROFILE) { return remote_port[profile]; };
    bool getAuthType(int profile = DEFAULT_PROFILE) { return auth_type[profile]; };
    std::string getUsername(int profile = DEFAULT_PROFILE) { return username[profile]; };
    std::string getPassword(int profile = DEFAULT_PROFILE) { return password[profile]; };
    bool getSSLEnabled(int profile = DEFAULT_PROFILE) { return ssl[profile]; };
    int getTimeout(int profile = DEFAULT_PROFILE) { return timeout[profile]; };
    int getCID(int profile = DEFAULT_PROFILE) { return cid[profile]; };
    int getPacketSize(int profile = DEFAULT_PROFILE) { return packet_size[profile]; };

    // HTTP Methods
    bool httpGET(std::string resource, std::string header = "", int profile = DEFAULT_PROFILE);
    bool httpHEAD(std::string resource, std::string header = "", int profile = DEFAULT_PROFILE);
    bool httpDELETE(std::string resource, std::string header = "", int profile = DEFAULT_PROFILE);
    bool httpPOST();
    bool httpPUT();
    bool receive();

    // Other utility functions
    void reset();

private:
    // The Telit LE910 can store up to 3 profiles
    std::string hostIP;
    std::string serverIP[3];
    int remote_port[3];
    int auth_type[3];
    std::string username[3];
    std::string password[3];
    bool ssl[3];
    int timeout[3];
    int cid[3];
    int packet_size[3];

    std::string httpGETBuf[3];
};

#endif
