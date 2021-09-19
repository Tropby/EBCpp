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

#include "../EBTcpSocket.hpp"

namespace EBCpp
{

/**
 * @brief Socket to handle a tcp connection
 *
 */
class EBSslSocket : public EBTcpSocket
{
public:
    /**
     * @brief Construct a new EBSslSocket object
     *
     */
    EBSslSocket()
    {
    }

    /**
     * @brief Construct a new EBSslSocket object
     *
     * @param ssl Ssl structure
     * @param socketId socket id of the tcp connection
     * @param client client informations
     */
    EBSslSocket(SSL* ssl, SOCKET socketId, struct sockaddr_in client) : EBTcpSocket(socketId, client), ssl(ssl)
    {
    }

    /**
     * @brief Destroy the EBSslSocket object
     *
     */
    virtual ~EBSslSocket()
    {
        // SSL_shutdown(ssl);
        close();
        joinThread();
        SSL_free(ssl);
        ssl = 0;
    }

    /**
     * @brief Send raw binary data
     *
     * @param data Pointer to the data
     * @param length Length of the data
     * @return int bytes written to the tcp socket
     */
    virtual int write(char* data, int length)
    {
        int len = SSL_write(ssl, data, length);
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
                EB_EMIT_WITH_ARGS(error, "SSL Error!");
                break;
            }
        }

        return len;
    }

    /**
     * @brief Send string
     *
     * @param data string to send
     * @return int bytes written to the tcp socket
     */
    virtual int write(const std::string& data)
    {
        int len = -1;
        try
        {
            if (isOpened())
            {
                len = SSL_write(ssl, data.c_str(), data.length());
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
                        EB_EMIT_WITH_ARGS(error, "SSL Error!");
                        break;
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        return len;
    }

protected:
    /**
     * @brief Creates a socket and connects to the host
     *
     * @return true if the socket is connected
     * @return false otherwise
     */
    virtual bool connect()
    {
        if (!EBTcpSocket::connect())
            return false;

        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        const SSL_METHOD* meth = TLS_client_method();
        SSL_CTX* ctx = SSL_CTX_new(meth);
        ssl = SSL_new(ctx);
        if (!ssl)
        {
            return false;
        }

        sslSock = SSL_get_fd(ssl);
        SSL_set_fd(ssl, socketId);
        int err = SSL_connect(ssl);
        if (err <= 0)
        {
            return false;
        }

        return true;
    }

    /**
     * @brief Receive data from the socket
     *
     * @param buffer buffer for the data
     * @param size size of the buffer
     * @return int bytes read from the socket
     */
    virtual int receiveData(char* buffer, int size)
    {
        // Read next block of data
        int nbytes = SSL_read(ssl, buffer, size);

        if (nbytes <= 0)
        {
            int error = SSL_get_error(ssl, nbytes);
            switch (error)
            {
            case SSL_ERROR_NONE:
                EB_LOG_ERROR("SSL_ERROR_NONE");
                break;
            case SSL_ERROR_ZERO_RETURN:
                EB_LOG_ERROR("SSL_ERROR_ZERO_RETURN");
                break;
            case SSL_ERROR_WANT_READ:
                EB_LOG_ERROR("SSL_ERROR_WANT_READ");
                break;
            case SSL_ERROR_WANT_WRITE:
                EB_LOG_ERROR("SSL_ERROR_WANT_WRITE");
                break;
            case SSL_ERROR_WANT_CONNECT:
                EB_LOG_ERROR("SSL_ERROR_WANT_CONNECT");
                break;
            case SSL_ERROR_WANT_ACCEPT:
                EB_LOG_ERROR("SSL_ERROR_WANT_ACCEPT");
                break;
            case SSL_ERROR_WANT_X509_LOOKUP:
                EB_LOG_ERROR("SSL_ERROR_WANT_X509_LOOKUP");
                break;
            case SSL_ERROR_WANT_ASYNC:
                EB_LOG_ERROR("SSL_ERROR_WANT_ASYNC");
                break;
            case SSL_ERROR_WANT_ASYNC_JOB:
                EB_LOG_ERROR("SSL_ERROR_WANT_ASYNC_JOB");
                break;
            case SSL_ERROR_SYSCALL:
                EB_LOG_ERROR("SSL_ERROR_SYSCALL");
                break;
            case SSL_ERROR_SSL:
                EB_LOG_ERROR("SSL_ERROR_SSL");
                break;
            }
        }

        return nbytes;
    }

private:
    SSL* ssl;
    int sslSock;
};

} // namespace EBCpp
