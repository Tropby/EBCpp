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


#include "ebsslclient.h"

#ifdef EB_OPENSSL

#include "../ebtcpclient.h"

using namespace EBCpp;

EBSSLClient::EBSSLClient() :
		EBTcpClient(), ssl(nullptr), sslSock(-1)
{

}

EBSSLClient::~EBSSLClient()
{
}

bool EBCpp::EBSSLClient::readRaw()
{
	int len;
	char buf[1024];

	len = SSL_read(ssl, buf, sizeof(buf));

	if( len > 0 )
	{
		std::vector<uint8_t> b(buf, buf + len);
		data.insert(data.end(), std::begin(b), std::end(b));
		readReady.emit(this->shared_from_this());
		return true;
	}

	else if( len == 0 )
	{
		disconnected.emit(this->shared_from_this());
		return false;
	}

	else
	{
		int err = SSL_get_error(ssl, len);
		error.emit(err);
		if (err == SSL_ERROR_WANT_READ)
			return false;
		if (err == SSL_ERROR_WANT_WRITE)
			return false;
		if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL
				|| err == SSL_ERROR_SSL)
			return false;
		return false;
	}
}

void EBCpp::EBSSLClient::logSslErrors()
{
    int err;
    while ( (err = ERR_get_error()) != 0 )
    {
        char *str = ERR_error_string(err, 0);
        if (!str)
            return;
        printf(str);
        printf("\n");
        fflush(stdout);
    }
}

bool EBCpp::EBSSLClient::connectRaw()
{
	if (!EBTcpSocket::connectRaw())
		return false;

	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	const SSL_METHOD *meth = TLS_client_method();
	SSL_CTX *ctx = SSL_CTX_new(meth);
	ssl = SSL_new(ctx);
	if (!ssl)
	{
		logSslErrors();
		return false;
	}

	sslSock = SSL_get_fd(ssl);
	SSL_set_fd(ssl, socketId);
	int err = SSL_connect(ssl);
	if (err <= 0)
	{
		logSslErrors();
		fflush(stdout);
		return false;
	}

	return true;
}

void EBCpp::EBSSLClient::write(std::string data)
{
	int len = SSL_write(ssl, data.c_str(), data.length());
	if (len < 0)
	{
		int err = SSL_get_error(ssl, len);
		switch (err)
		{
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_SYSCALL:
		case SSL_ERROR_SSL:
		default:
			error.emit(err);
			break;
		}
	}
}

#endif
