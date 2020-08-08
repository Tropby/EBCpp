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

#include <thread>
#include <memory>
#include "ebtcpserversocket.h"

namespace EBCpp
{

EBTcpServerSocket::EBTcpServerSocket(SOCKET socketId) :
		enable_shared_from_this<EBTcpServerSocket>(), socketId(socketId), deleted(false), thread(nullptr)
{

}

EBTcpServerSocket::~EBTcpServerSocket()
{
}

std::vector<uint8_t> EBTcpServerSocket::read()
{
	std::vector<uint8_t> ret = data;
	data.clear();
	return ret;
}

std::string EBTcpServerSocket::readString()
{
	std::vector<uint8_t> r = this->read();
	r.insert(r.end(), 0x00);
	return std::string(reinterpret_cast<char*>(r.data()));
}

void EBTcpServerSocket::write(std::string data)
{
	send(socketId, data.c_str(), data.length(), 0);
}

void EBTcpServerSocket::write(char *data, int size)
{
	send(socketId, data, size, 0);
}

void EBTcpServerSocket::close()
{
	this->deleted = true;

	::close(socketId);
	if( thread.get() != nullptr )
	{
		thread->join();
		thread = nullptr;
	}
}

void EBTcpServerSocket::start()
{
	if( thread.get() != nullptr )
	{
		std::cerr << "DO NOT START SOCKET"  << std::endl;
		return;
	}
	thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBTcpServerSocket::readLoop, this)));
}

void EBTcpServerSocket::readLoop()
{
	std::shared_ptr<EBTcpServerSocket> pthis = this->shared_from_this();

	char buffer[1024];
	int nbytes;
	while(!deleted)
	{
		nbytes = ::recv(socketId, buffer, sizeof(buffer), 0);
		if( deleted )
			return;

		switch (nbytes)
		{
		case 0:
			disconnected.emit(pthis);
			return;
		case -1:
			error.emit(pthis);
			return;
		default:
			std::vector<uint8_t> b(buffer, buffer + nbytes);
			data.insert(data.end(), std::begin(b), std::end(b));
			readReady.emit(pthis);
		}
	}
	std::cout << "Thread ENDED!" << std::endl;
}

} /* namespace EBCpp */
