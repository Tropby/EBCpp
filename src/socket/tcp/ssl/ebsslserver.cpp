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
 *  Created on: Sep 20, 2020
 *      Author: Carsten (Tropby)
 */

#include "ebsslserver.h"
#include "../../../ebconfig.h"

#ifdef EB_OPENSSL

namespace EBCpp
{

EBSSLServer::EBSSLServer() :
		EBServer()
{
#ifdef __WIN32__
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(versionWanted, &wsaData);
#endif

	socketId = socket( AF_INET, SOCK_STREAM, 0);

	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	const SSL_METHOD *method;

	method = SSLv23_server_method();

	ctx = SSL_CTX_new(method);
	if (!ctx)
	{
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stdout);
		exit(EXIT_FAILURE);
	}

	SSL_CTX_set_ecdh_auto(ctx, 1);

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0)
	{
		perror("Can not load CERT.pem");
		ERR_print_errors_fp(stdout);
		exit(EXIT_FAILURE);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0)
	{
		perror("Can not load KEY.pem");
		ERR_print_errors_fp(stdout);
		exit(EXIT_FAILURE);
	}
}

EBSSLServer::~EBSSLServer()
{
	SSL_CTX_free(ctx);
	EVP_cleanup();
}

bool EBSSLServer::bind(uint16_t port)
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
	thread = std::unique_ptr<std::thread>(
			new std::thread(std::bind(&EBSSLServer::acceptConnections, this)));
	return true;
}

void EBSSLServer::unbind()
{
	deleted = true;
	::close(socketId);
	if (thread.get() != nullptr)
	{
		thread->join();
		std::thread *t = thread.release();
		thread.get_deleter()(t);
		thread = nullptr;
	}
}

void EBSSLServer::acceptConnections()
{
	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);

	while (!deleted)
	{
		int connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli),
				&len);
		if (connfd >= 0)
		{
			SSL *ssl;
			ssl = SSL_new(ctx);
			SSL_set_fd(ssl, connfd);
			if (SSL_accept(ssl) > 0)
			{
				std::shared_ptr<EBSSLServerSocket> socket = std::make_shared<
						EBSSLServerSocket>(ssl, connfd);
				socket->start();
				newConnection.emit(socket);
			}
		}
	}
}

} /* namespace EBCpp */

#endif
