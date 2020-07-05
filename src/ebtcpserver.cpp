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

#include "ebtcpserver.h"

#include <cstring>

using namespace EBCpp;

EBTcpServer::EBTcpServer() :
		EBTcpSocket()
{
}

EBTcpServer::~EBTcpServer()
{
	close();
}

bool EBTcpServer::bind(uint16_t port)
{
	struct sockaddr_in servaddr;

	memset(&servaddr, 0, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if ((::bind(socketId, reinterpret_cast<sockaddr*>(&servaddr),
			sizeof(servaddr))) != 0)
	{
		printf("socket bind failed... %d\n", socketId);
		return false;
	}
	if ((listen(socketId, 5)) != 0)
	{
		printf("Listen failed...\n");
		return false;
	}

	processConnect.release();
	return true;
}

bool EBTcpServer::acceptRaw()
{
	struct sockaddr_in cli;

	int len = sizeof(cli);
	int connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli), &len);
	if (connfd < 0)
	{
		printf("server acccept failed...\n");
		return false;
	}

	std::shared_ptr<EBTcpClient> socket = std::make_shared<EBTcpClient>(connfd,
			true);

	clients.push_back( socket );

	newConnection.emit(socket);

	return true;
}

bool EBTcpServer::runRaw()
{
	state = false;
	processConnect.acquire();
	if (deleted)
		return false;

	state = true;
	while (!deleted && acceptRaw())
	{
	}
	return true;
}
