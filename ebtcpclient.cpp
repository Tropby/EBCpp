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

#include "ebtcpclient.h"


using namespace EBCpp;

EBTcpClient::EBTcpClient(int socketId, bool connected) : EBTcpSocket(socketId, connected)
{
}

EBTcpClient::~EBTcpClient()
{
	close();
	std::cout << "EBTcpClient::~EBTcpClient()" << std::endl;
}

void EBTcpClient::connectToHost(std::string ip, uint16_t port)
{
	std::string hostIp = hostnameToIp(ip);
	if (!hostIp.empty())
		ip = hostIp;

	struct in_addr addr;
//	inet_aton(ip.c_str(), &addr);
	inet_pton(AF_INET, ip.c_str(), &addr);
	connectToHost(addr.s_addr, port);
}

void EBTcpClient::connectToHost(uint32_t ip, uint16_t port)
{
	this->ip = ip;
	this->port = port;
	processConnect.release();
}

std::vector<uint8_t> EBTcpClient::read()
{
	std::vector<uint8_t> ret = data;
	data.clear();
	return ret;
}

std::string EBTcpClient::readString()
{
	std::vector<uint8_t> r = this->read();
	r.insert( r.end(), 0x00);
	return std::string(reinterpret_cast<char*>(r.data()));
}

void EBTcpClient::write(std::string data)
{
	send(socketId, data.c_str(), data.length(), 0);
}

void EBTcpClient::write(char * data, int size )
{
	send(socketId, data, size, 0);
}

bool EBTcpClient::readRaw()
{
	char buffer[1024];
	int nbytes;
	nbytes = ::read(socketId, buffer, sizeof(buffer));

	switch (nbytes)
	{
	case 0:
		disconnected.emit(this);
		return false;
	case -1:
		error.emit(2);
		return false;
	default:
		std::vector<uint8_t> b(buffer, buffer + nbytes);
		data.insert(data.end(), std::begin(b), std::end(b));
		readReady.emit(this);
		return true;
	}
}

bool EBTcpClient::connectRaw()
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = (ip);
	addr.sin_port = htons(port);

	int descriptor = ::connect(socketId, reinterpret_cast<sockaddr*>(&addr),
			sizeof(addr));
	if (descriptor == -1)
	{
		return false;
	}
	return true;
}

bool EBCpp::EBTcpClient::runRaw()
{
	if( !initialConnected )
	{
		state = false;

		// Wait for setup socket
		processConnect.acquire();
		if (deleted)
			return false;

		if (!connectRaw())
		{
			error.emit(1);
			return false;
		}

		state = true;
		connected.emit(this);
	}
	else
	{
		state = true;
	}

	while (!deleted && readRaw())
	{
	}

	return !deleted;
}
