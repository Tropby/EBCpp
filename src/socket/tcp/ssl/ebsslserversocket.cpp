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
#include "ebsslserversocket.h"

#ifdef EB_OPENSSL

namespace EBCpp
{

EBSSLServerSocket::EBSSLServerSocket(SSL *ssl, SOCKET socketId) :
		EBServerSocket(socketId), data(
		{ }), deleted(false), thread(nullptr), opened(true), ssl(ssl)
{
	EB_DEBUG(socketId);

	linger l;
	l.l_onoff = 1;
	l.l_linger = 10;
	::setsockopt(socketId, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&l),
			sizeof(l));
}

EBSSLServerSocket::~EBSSLServerSocket()
{
	EB_DEBUG(socketId);

	SSL_shutdown(ssl);
	SSL_free(ssl);

#ifdef __WIN32__
	::closesocket(socketId);
#else
	::close(socketId);
#endif
}

std::vector<uint8_t> EBSSLServerSocket::read()
{
	std::vector<uint8_t> ret;
	if (data.size())
	{
		ret = data;
		data.clear();
	}
	return ret;
}

std::string EBSSLServerSocket::readString()
{
	std::vector<uint8_t> r = this->read();
	r.insert(r.end(), 0x00);
	return std::string(reinterpret_cast<char*>(r.data()));
}

void EBSSLServerSocket::write(std::string data)
{
	SSL_write(ssl, data.c_str(), data.length());
}

void EBSSLServerSocket::write(char *data, int size)
{
	SSL_write(ssl, data, size);
}

void EBSSLServerSocket::close()
{
	if (opened.exchange(false))
	{
		::shutdown(socketId, SD_SEND);
		thread->join();
	}
}

void EBSSLServerSocket::start()
{
	if (thread.get() != nullptr)
	{
		return;
	}
	thread = std::make_shared<std::thread>(
			std::bind(&EBSSLServerSocket::readLoop, this));
}

void EBSSLServerSocket::readLoop()
{
	std::shared_ptr<EBServerSocket> pthis = this->shared_from_this();

	char buffer[1024];
	int nbytes;
	while (!deleted)
	{
		nbytes = SSL_read(ssl, buffer, sizeof(buffer));
		if (deleted)
			break;

		switch (nbytes)
		{
		case 0:
			disconnected.emit(pthis);
			deleted = true;
			break;
		case -1:
			error.emit(pthis);
			deleted = true;
			break;
		default:
			std::vector<uint8_t> b(buffer, buffer + nbytes);
			data.insert(data.end(), std::begin(b), std::end(b));
			readReady.emit(pthis);
			break;
		}
	}
}

} /* namespace EBCpp */

#endif
