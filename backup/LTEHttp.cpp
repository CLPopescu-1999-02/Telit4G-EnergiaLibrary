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

#include "LTEBase.h"
#include "LTEHttp.h"


/** LTE Http class constructor. Sets private variables to their
 *  default/initial vaues.
 *
 *  @param  tp  Telit Serial port.
 *  @param  dp  Debug Serial port pointer.
 */
LTEHttp::LTEHttp(HardwareSerial* tp, HardwareSerial* dp) : LTEBase::LTEBase(tp, dp) {
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


// TODO: test
/** Sets the remote IP address to "xxx.xxx.xxx.xxx" for the given HTTP profile.
 *  For example, the function call setAddress(5, 4, 3, 2, 1) would result in the
 *  IP address "005.004.003.002" to be set to HTTP profile 1.
 *
 *  @param  ip1         First (most significant) 8 bits in human readable form.
 *  @param  ip2         Second 8 bits in humal readable form.
 *  @param  ip3         Second to last 8 bits in human readable form.
 *  @param  ip4         Last 8 bits in human readable form.
 *  @param  profile     HTTP profile number to save the IP address to.
 *  @return bool        True on success.
 */
bool LTEHttp::setAddress(int ip1, int ip2, int ip3, int ip4, int profile) {
    if ((ip1 < 0) || (ip1 > 255) ||
        (ip2 < 0) || (ip2 > 255) ||
        (ip3 < 0) || (ip3 > 255) ||
        (ip4 < 0) || (ip4 > 255)) return false;
    if ((profile < 0) || (profile > 2)) return false;

    char buf[4];
    std::string temp = "";

    sprintf(buf, "%03d", ip1);
    temp += std::string(buf) + ".";
    sprintf(buf, "%03d", ip2);
    temp += std::string(buf) + ".";
    sprintf(buf, "%03d", ip3);
    temp += std::string(buf) + ".";
    sprintf(buf, "%03d", ip4);
    temp += std::string(buf);

    serverIP[profile] = temp;
    return true;
}


// TODO: test
/** Sets the remote address to a URL to be solved by a DNS query later. For
 *  example, the function call setAddress("www.google.com", 1) would set the
 *  remote address to "www.google.com" for HTTP profile 1.
 *
 *  @param  hostname    Remote URL as a string.
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setAddress(std::string hostname, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (hostname == "") return false;
    serverIP[profile] = hostname;
    return true;
}


// TODO: test
/** Sets the remote port to the given value, for the given profile. The
 *  default port is 80 for general cases, and 9978 for "m2mlocate.telit.com".
 *
 *  @param  rp          Remote port.
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setServerPort(int rp, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if ((rp < 0) || (rp > 65535)) return false;
    remote_port[profile] = rp;
    return true;
}


// TODO: test
/** Indicates HTTP authentication type for the given HTTP profile. If basic
 *  authentication is required, also allows you to enter a username and password.
 *
 *  @param  auth        Boolean indicating authentication type. True is basic
 *                      authentication, and false is no authentication.
 *  @param  usr         Username if basic authentication is chosen.
 *  @param  pass        Password if basic authentication is chosen. Defaults to "".
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setAuthentication(bool auth, std::string usr, std::string pass, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (auth == true) {
        auth_type[profile] = 1;
        username[profile] = usr;
        password[profile] = pass;
    }
    else auth_type[profile] = 0;
    return true;
}


// TODO: test
/** Indicates whether SSL encryption is enabled.
 *  
 *  @param  s           Boolean indicating whether SSL is enabled. True is enabled.
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setSSLEnabled(bool s, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    ssl[profile] = s;
    return false;
}


// TODO: test
/** Sets the timeout value (time interval to wait for receiving data from the
 *  HTTP server) in seconds. Range is between 1-65535, and the default is 120s.
 *
 *  @param  timeo       Time interval to wait in seconds.
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setTimeout(int timeo, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if ((timeo < 0) || (timeo > 65535)) return false;
    timeout[profile] = timeo;
    return true;
}


// TODO: test
/** Indicates the PDP Context Identifier for the given HTTP profile. The range
 *  is found by the AT test command "AT#HTTPCFG=?" under the CID field. The
 *  default value is 3.
 *
 *  @param  c           PDP Context Identifier.
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setCID(int c, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if ((c < 0) || (c > 15)) return false;  // 15 is the max CID given by the
                                                // AT#HTTPCFG=? command
    cid[profile] = c;
    return true;
}


// TODO: test
/** Sets the max packet size (in bytes) allowed when sending or receiving.
 *
 *  @param  ps          Packet size in bytes.
 *  @param  profile     HTTP profile to save to.
 *  @return bool        True on success.
 */
bool LTEHttp::setPacketSize(int ps, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if ((ps < 0) || (ps > 1500)) return false;
    packet_size[profile] = ps;
    return true;
}


// TODO: test
/** Sends an HTTP GET request for the resource at the specified profile.
 *  You can use the header field to include any additional headers you would
 *  like, however "Connection: close" is always included by default.
 *
 *  @param  resource    Resource requested.
 *  @param  header      Additional headers to be included in the request.
 *  @param  profile     HTTP profile to use to send the request.
 *  @return bool        True on success.
 */
bool LTEHttp::httpGET(std::string resource, std::string header, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (resource == "") return false;

    std::string cmd = "AT#HTTPQRY=" + std::to_string(profile) +
                      ",0," + resource;
    if (header != "") cmd += "," + header;
    if (!getCommandOK(cmd)) return false;
    return true;
}


// TODO: test
/** Sends an HTTP HEAD request for the resource at the specified profile.
 *  You can use the header field to include any additional headers you would
 *  like, however "Connection: close" is always included by default.
 *
 *  @param  resource    Resource requested.
 *  @param  header      Additional headers to be included in the request.
 *  @param  profile     HTTP profile to use to send the request.
 *  @return bool        True on success.
 */
bool LTEHttp::httpHEAD(std::string resource, std::string header, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (resource == "") return false;

    std::string cmd = "AT#HTTPQRY=" + std::to_string(profile) +
                      ",1," + resource;
    if (header != "") cmd += "," + header;
    if (!getCommandOK(cmd)) return false;
    return true;
}


// TODO: test
/** Sends an HTTP DELETE request for the resource at the specified profile.
 *  You can use the header field to include any additional headers you would
 *  like, however "Connection: close" is always included by default.
 *
 *  @param  resource    Resource requested.
 *  @param  header      Additional headers to be included in the request.
 *  @param  profile     HTTP profile to use to send the request.
 *  @return bool        True on success.
 */
bool LTEHttp::httpDELETE(std::string resource, std::string header, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (resource == "") return false;

    std::string cmd = "AT#HTTPQRY=" + std::to_string(profile) +
                      ",2," + resource;
    if (header != "") cmd += "," + header;
    if (!getCommandOK(cmd)) return false;
    return true;
}


// TODO: test
/** Sends an HTTP POST request at the given resource for the specified profile.
 *  You can use the header field to include any additional headers you would 
 *  like, however, "Connection: close" is always included by default. The content
 *  field contains the content to be posted. The post_param field is a string
 *  that indicates the HTTP Content-identifier, and is represented by an integer
 *  with an optional following colon character and string representing subtypes
 *  for that identifier. The post_param field has the following range of values
 *  (where the value inside square brackets is optional) and corresponding
 *  Content-identifiers:
 *      "0[:extension]" >   "application/x-www-form-urlencoded"
 *      "1[:extension]" >   "text/plain"
 *      "2[:extension]" >   "application/octet-stream"
 *      "3[:extension]" >   "multipart/form-data"
 *      "[any string]"  >   Use this to enter other string values for
 *                          the Content-identifier field
 *
 *  @param  resource    String containing the resource requested.
 *  @param  content     String containing body of the POST request.
 *  @param  post_param  String representing the Content-identifer header field.
 *  @param  header      Additional headers to be included in the request.
 *  @param  profile     HTTP profile to use to send the request.
 *  @return bool        True on success.
 */
bool LTEHttp::httpPOST(std::string resource, std::string content, std::string post_param, std::string header, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (resource == "") return false;

    std::string cmd = "AT#HTTPSND=" + std::to_string(profile) +
                      ",0," + resource + "," + std::to_string(content.length());
    if ((header == "") && (post_param == "")) {
        if (!getCommandOK(cmd)) return false;
    }
    else {
        cmd += ",";
        if (post_param != "") cmd += content;
        if (header != "") cmd += "," + header;
    }
    return true;
}


// TODO: test
/** Sends an HTTP PUT request at the given resource for the specified profile.
 *  You can use the header field to include any additional headers you would 
 *  like, however, "Connection: close" is always included by default. The content
 *  field contains the content to be posted. The put_param field is a string
 *  that indicates the HTTP Content-identifier, and is represented by an integer
 *  with an optional following colon character and string representing subtypes
 *  for that identifier. The post_param field has the following range of values
 *  (where the value inside square brackets is optional) and corresponding
 *  Content-identifiers:
 *      "0[:extension]" >   "application/x-www-form-urlencoded"
 *      "1[:extension]" >   "text/plain"
 *      "2[:extension]" >   "application/octet-stream"
 *      "3[:extension]" >   "multipart/form-data"
 *      "[any string]"  >   Use this to enter other string values for
 *                          the Content-identifier field
 *
 *  @param  resource    String containing the resource requested.
 *  @param  content     String containing body of the POST request.
 *  @param  post_param  String representing the Content-identifer header field.
 *  @param  header      Additional headers to be included in the request.
 *  @param  profile     HTTP profile to use to send the request.
 *  @return bool        True on success.
 */
bool LTEHttp::httpPOST(std::string resource, std::string content, std::string post_param, std::string header, int profile) {
    if ((profile < 0) || (profile > 2)) return false;
    if (resource == "") return false;

    std::string cmd = "AT#HTTPSND=" + std::to_string(profile) +
                      ",0," + resource + "," + std::to_string(content.length());
    if ((header == "") && (post_param == "")) {
        if (!getCommandOK(cmd)) return false;
    }
    else {
        cmd += ",";
        if (post_param != "") cmd += content;
        if (header != "") cmd += "," + header;
    }
    return true;
}


// TODO: finish
bool LTEHttp::receive() {
    receiveData();
    return false;
}


/** Resets private variables to their default values.
 *
 *  @return void
 */
void LTEHttp::reset(int profile) {
    hostIP = "";
    for (int x = 0; x < 3; x++) {
        serverIP[x] = "";
        remote_port[x] = 0;
        auth_type[x] = 0;
        username[x] = "";
        password[x] = "";
        ssl[x] = false;
        timeout[x] = 120;
        cid[x] = 3;
        packet_size[x] = 0;
        httpGETBuf[x] = "";
    }
    serverIP[2] = "m2mlocate.telit.com";
    remote_port[2] = 9978;
}

#endif
