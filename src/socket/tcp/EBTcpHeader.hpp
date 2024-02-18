/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#if defined(__WIN32__) || defined(_WIN32)
    #include <io.h>

    #include "../../EBOs.hpp"
    
    #include <winsock2.h>
    #include <ws2def.h>
    #include <ws2tcpip.h>
    #include <ws2ipdef.h>
    #include <fcntl.h>

    // Define TCP Keepalive types
    #ifndef TCP_KEEPCNT
        #define TCP_KEEPCNT 8
        #define TCP_KEEPINTVL 150
        #define TCP_KEEPIDLE 14400
    #endif
#else
    #include <arpa/inet.h>
    #include <netdb.h> //hostent
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <cstring>
    #include <unistd.h>
    #include <fcntl.h>
    #define SOCKADDR_IN sockaddr_in
    #define SOCKET int32_t
#endif