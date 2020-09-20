/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
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
 *  Created on: Jul 3, 2020
 *      Author: Carsten (Tropby)
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <cstdint>
#include <vector>
#include <cstring>
#include <memory>

#ifdef __WIN32__
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>	//hostent
	#include <arpa/inet.h>
	#define SOCKET int32_t
#endif

#include <stdio.h>
#include <unistd.h>



#include "../../ebconfig.h"
#include "../../ebevent.h"

namespace EBCpp{

class EBTcpSocket
{
public:
    EBTcpSocket(int socketId = -1, bool connected = false);
    virtual ~EBTcpSocket();

    EB_SIGNAL(error, int);

    std::string hostnameToIp( std::string hostname );

    void close();

    bool isConnected(){return state;}

protected:
    SOCKET socketId;


    uint32_t ip;
    uint16_t port;

    std::vector<uint8_t> data;

    void startThread();
    void stopThread();

    void run();

    virtual bool connectRaw();
    virtual bool readRaw();
    virtual bool acceptRaw();
    virtual bool runRaw();

    bool deleted;
    bool state;
    bool initialConnected;

    static bool inited;

    std::shared_ptr<std::thread> thread;
};

}

#endif // SOCKET_H
