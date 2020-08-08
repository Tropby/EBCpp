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


#ifndef SRC_TCP_EBTCPSERVERSOCKET_H_
#define SRC_TCP_EBTCPSERVERSOCKET_H_

#include <string>
#include <vector>
#include <atomic>
#include <memory>

#include "../ebevent.h"

#include "tcp.h"

namespace EBCpp
{

class EBTcpServerSocket : public std::enable_shared_from_this<EBTcpServerSocket>
{
public:
	EBTcpServerSocket( SOCKET socketId );
	virtual ~EBTcpServerSocket();

    EB_SIGNAL(disconnected, std::shared_ptr<EBTcpServerSocket>);
    EB_SIGNAL(error, std::shared_ptr<EBTcpServerSocket>);
    EB_SIGNAL(readReady, std::shared_ptr<EBTcpServerSocket>);

    void close();
    void start();

	void write(std::string data);
	void write(char * data, int size );
	std::vector<uint8_t> read();
	std::string readString();

private:
	SOCKET socketId;
	std::vector<uint8_t> data;
	std::atomic<bool> deleted;
	std::unique_ptr<std::thread> thread;

	void readLoop();
};

} /* namespace EBCpp */

#endif /* SRC_TCP_EBTCPSERVERSOCKET_H_ */