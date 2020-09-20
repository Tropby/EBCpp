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
#include <cstring>

#include "ebtcpserver.h"

using namespace EBCpp;

EBTcpServer::EBTcpServer() :
		thread(nullptr), deleted(false), socketId(-1)
{
#ifdef __WIN32__
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(versionWanted, &wsaData);
#endif

	socketId = socket( AF_INET, SOCK_STREAM, 0);
}

EBTcpServer::~EBTcpServer()
{
	std::cout << "EBTcpServer::~EBTcpServer() -> unbind" << std::endl;
	unbind();
	std::cout << "EBTcpServer::~EBTcpServer() -> finished" << std::endl;
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
		return false;
	}
	if ((::listen(socketId, 5)) != 0)
	{
		return false;
	}

	deleted = false;
	thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBTcpServer::acceptConnections, this)));
	return true;
}

void EBCpp::EBTcpServer::unbind()
{
	deleted = true;
	::close(socketId);
	if (thread.get() != nullptr)
	{
		thread->join();
		std::thread * t = thread.release();
		thread.get_deleter() ( t );
		thread = nullptr;
	}
}

void EBTcpServer::acceptConnections()
{
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);

	while (!deleted)
	{
		int connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli),
				&len);
		if (connfd >= 0)
		{
			std::shared_ptr<EBTcpServerSocket> socket = std::make_shared<
					EBTcpServerSocket>(connfd);
			socket->start();
			newConnection.emit(socket);
		}
	}
}
