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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#ifdef EB_OPEN_SSL

#include <openssl/ssl.h> 
#include <openssl/err.h>

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
     * @param parent Parent of the EBSslSocket instance
     */
    EBSslSocket( EBObject* parent ) : 
        EBTcpSocket(parent)
    {
    }

    /**
     * @brief Construct a new EBSslSocket object
     * 
     * @param ssl Ssl structure
     * @param parent Parent of the EBSslSocket instance
     */
    EBSslSocket( EBObject* parent, SSL* ssl, SOCKET socketId, struct sockaddr_in client ) : 
        EBTcpSocket(parent, socketId, client),
        ssl(ssl)
    {
    }

    /**
     * @brief Destroy the EBSslSocket object
     * 
     */
    virtual ~EBSslSocket()
    {
     
    }

    /**
     * @brief Send raw binary data
     * 
     * @param data Pointer to the data
     * @param length Length of the data
     * @return int bytes written to the tcp socket
     */
    virtual int write( char * data, int length )
    {
        return send(socketId, data, length, 0);
    }

    /**
     * @brief Send string
     * 
     * @param data string to send
     * @return int bytes written to the tcp socket
     */
    virtual int write( std::string data )
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
                EB_EMIT_WITH_ARGS(error, "SSL Error!");
                break;
            }
        }

        return len;
    }

    /**
     * @brief Creates a socket and connects to the host
     * 
     * @return true if the socket is connected 
     * @return false otherwise
     */
    virtual bool connect()
    {
        if( !EBTcpSocket::connect() )
            return false;

        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        const SSL_METHOD *meth = TLS_client_method();
        SSL_CTX *ctx = SSL_CTX_new(meth);
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
    virtual int receiveData(char * buffer, int size)
    {
        // Read next block of data
        return SSL_read(ssl, buffer, size);
    }    

private:
    SSL *ssl;
    int sslSock;

};

}

#endif