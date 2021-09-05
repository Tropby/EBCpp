/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten (Tropby)
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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <cinttypes>
#include <functional>
#include <memory>
#include <thread>

#include "../EBTcpServer.hpp"
#include "../EBTcpSocket.hpp"

#include "EBSslSocket.hpp"

namespace EBCpp
{

/**
 * @brief Creates a TCP server
 *
 */
class EBSslServer : public EBTcpServer
{
public:
    /**
     * @brief Construct a new EBTcpServer object
     *
     * @param certFileName Cert file name
     * @param keyFileName Key file name
     * @param parent Parent object of the tcp server
     */
    EBSslServer(EBObject* parent, std::string certFileName, std::string keyFileName) : EBTcpServer(parent)
    {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        const SSL_METHOD* method;
        method = TLS_server_method();

        ctx = SSL_CTX_new(method);
        if (!ctx)
        {
            perror("Unable to create SSL context");
            ERR_print_errors_fp(stdout);
            exit(EXIT_FAILURE);
        }

        /* Set the key and cert */
        if (SSL_CTX_use_certificate_file(ctx, certFileName.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            perror("Can not load CERT.pem");
            ERR_print_errors_fp(stdout);
            exit(EXIT_FAILURE);
        }

        if (SSL_CTX_use_PrivateKey_file(ctx, keyFileName.c_str(), SSL_FILETYPE_PEM) <= 0)
        {
            perror("Can not load KEY.pem");
            ERR_print_errors_fp(stdout);
            exit(EXIT_FAILURE);
        }
    }

    ~EBSslServer()
    {
        SSL_CTX_free(ctx);
        EVP_cleanup();
    }

protected:
    virtual EBTcpSocket* nextConnection()
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        int connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli), &len);
        if (connfd >= 0)
        {
            SSL* ssl;
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, connfd);
            int state = SSL_accept(ssl);
            if (state > 0)
            {
                EBSslSocket* socket = new EBSslSocket(this, ssl, connfd, cli);
                socket->startThread();
                return socket;
            }
            else
            {
                int err = SSL_get_error(ssl, state);

                if (err == SSL_ERROR_WANT_READ)
                {
                    /* Wait for data to be read */
                }
                else if (err == SSL_ERROR_WANT_WRITE)
                {
                    /* Write data to continue */
                }
                else if (err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
                {
                    /* Hard error */
                }
                else if (err == SSL_ERROR_ZERO_RETURN)
                {
                    /* Same as error */
                }
#ifdef __WIN32__                
                closesocket(connfd);
#else                
                close(connfd);
#endif                
                SSL_free(ssl);
                return nullptr;
            }
        }

        return nullptr;
    }

private:
    SSL_CTX* ctx;
};

} // namespace EBCpp
