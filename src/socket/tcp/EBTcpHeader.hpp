/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten (Tropby)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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