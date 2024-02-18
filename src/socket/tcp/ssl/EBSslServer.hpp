/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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
     */
    EBSslServer(std::string certFileName, std::string keyFileName)
    {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        const SSL_METHOD* method;
        method = TLSv1_2_server_method();

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
    virtual EBObjectPointer<EBTcpSocket> nextConnection()
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
                EBObjectPointer<EBSslSocket> socket = this->createObject<EBSslSocket>(ssl, connfd, cli);
                socket->startThread();
                return socket->template cast < EBTcpSocket >();
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
