/*
 * Copyright (c) 2016 by Wenlong Xiong <wenlongx@ucla.edu>
 * 4G/LTE Library for Telit LE910SV module and Energia.
 *
 * This library is adapted from the GSM Arduino library provided by
 * Justin Downs (2010) at
 * http://wiki.groundlab.cc/doku.php?id=gsm_arduino_library
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


 #ifndef LTE_LTE_HTTP_H_
 #define LTE_LTE_HTTP_H_

 #include "LTE_Base.h"


class LTEHttp : public LTEBase {
public:
    LTEHttp(HardwareSerial &telitPort, HardwareSerial* debugPort);
    bool init(uint16_t lte_band);

    // HTTP Methods
    httpGET();
    httpHEAD();
    httpDELETE();
    httpPOST();
    httpPUT();
    receive();

private:
    uint32_t hostIP;
    uint32_t serverIP;


    char* getBuf;
}

 #endif
