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
 *  Created on: Sep 22, 2020
 *      Author: Carsten (Tropby)
 */

#ifndef SRC_SOCKET_EBSERVERSOCKET_H_
#define SRC_SOCKET_EBSERVERSOCKET_H_

#include <vector>
#include "../ebevent.h"
#include "tcp/tcp.h"

namespace EBCpp
{

class EBServerSocket : public std::enable_shared_from_this<EBServerSocket>
{
public:
	EBServerSocket(SOCKET socketId) : socketId(socketId){}
	virtual ~EBServerSocket(){}

    EB_SIGNAL(disconnected, std::shared_ptr<EBServerSocket>);
    EB_SIGNAL(error, std::shared_ptr<EBServerSocket>);
    EB_SIGNAL(readReady, std::shared_ptr<EBServerSocket>);

    virtual void close() = 0;
    virtual void start() = 0;

	virtual void write(std::string data) = 0;
	virtual void write(char * data, int size ) = 0;
	virtual std::vector<uint8_t> read() = 0;
	virtual std::string readString() = 0;

	SOCKET getSocketId(){ return socketId; }

protected:
	SOCKET socketId;
};

} /* namespace EBCpp */

#endif /* SRC_SOCKET_EBSERVERSOCKET_H_ */
