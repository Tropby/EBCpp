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

#include <iostream>

#include "ebtcpsocket.h"

using namespace EBCpp;

bool EBTcpSocket::inited = false;

EBTcpSocket::EBTcpSocket(int socketId, bool connected) :
		socketId(socketId), thread(std::bind(&EBTcpSocket::run, this)), deleted(
				false), state(false), initialConnected(connected)
{
	if (!inited)
	{
#ifdef __WIN32__
		WORD versionWanted = MAKEWORD(1, 1);
		WSADATA wsaData;
		WSAStartup(versionWanted, &wsaData);
#endif
		inited = true;
	}
	if (socketId == -1)
	{
		this->socketId = socket( AF_INET, SOCK_STREAM, 0);
	}
}

EBTcpSocket::~EBTcpSocket()
{
	deleted = true;
	close();
	thread.join();
}

void EBCpp::EBTcpSocket::close()
{
	if (isConnected())
	{
#ifdef __WIN32__
		::shutdown(socketId, SD_BOTH);
#else
		::shutdown(socketId, SHUT_RDWR);
#endif
		::close(socketId);
	}
}

void EBCpp::EBTcpSocket::run()
{
	// Wait until all is inited.
	// Otherwise the rawRun method could be "pure virtual"
	while (!inited)
		usleep(10000);

	while (runRaw())
		;
}

std::string EBCpp::EBTcpSocket::hostnameToIp(std::string hostname)
{
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname.c_str(), "http", &hints, &servinfo)) != 0)
	{
		return "";
	}

	// loop through all the results and connect to the first we can
	char ip[64];
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		h = (struct sockaddr_in*) p->ai_addr;
		strcpy(ip, inet_ntoa(h->sin_addr));
	}

	freeaddrinfo(servinfo); // all done with this structure
	return ip;

}

bool EBCpp::EBTcpSocket::acceptRaw()
{
	return false;
}

bool EBCpp::EBTcpSocket::connectRaw()
{
	return false;
}

bool EBCpp::EBTcpSocket::readRaw()
{
	return false;
}
