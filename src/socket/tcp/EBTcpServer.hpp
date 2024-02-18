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

#include <cinttypes>
#include <functional>
#include <memory>
#include <thread>

#include "../../profile/EBProfile.hpp"
#include "../../EBEvent.hpp"
#include "../../EBObject.hpp"

#include "EBTcpHeader.hpp"
#include "EBTcpSocket.hpp"

namespace EBCpp
{

/**
 * @brief Creates a TCP server
 *
 */
class EBTcpServer : public EBObject <EBTcpServer>
{
public:
    /**
     * @brief Construct a new EBTcpServer object
     *
     * @param parent Parent object of the tcp server
     */
    EBTcpServer() :  socketId(-1), bound(false), thread(nullptr)
    {
        EB_PROFILE_FUNC();
    }

    /**
     * @brief Binds the server socket on a port
     *
     * @param port Port to bind the server socket
     * @param bindIp Ip to bind the server on
     * @return true if the server socket could be bind
     * @return false if the binding fails
     */
    virtual bool bind(uint16_t port, std::string bindIp = "0.0.0.0")
    {
        EB_PROFILE_FUNC();

        if (thread != nullptr)
            return false;

        EBUtils::startupTCP();

        socketId = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in servaddr;

        memset(&servaddr, 0, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(bindIp.c_str());
        servaddr.sin_port = htons(port);

        if ((::bind(socketId, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr))) != 0)
        {
            return false;
        }
        if ((::listen(socketId, 5)) != 0)
        {
            return false;
        }

        bound = true;
        thread = new std::thread(std::bind(&EBTcpServer::acceptConnections, this));
        return true;
    }

    /**
     * @brief Unbind the server socket from the port
     */
    void unbind()
    {
        EB_PROFILE_FUNC();

        bound = false;
        ::close(socketId);

        if (thread != nullptr)
        {
            thread->join();
            thread = nullptr;
        }
    }

    /**
     * @brief EB_SIGNAL newConnection
     *
     * This signal will be called for each tcp connection that will be established
     */
    EB_SIGNAL_WITH_ARGS(newConnection, EBObjectPointer<EBTcpSocket>);

protected:
    //! current socket id
    SOCKET socketId;

    /**
     * @brief Get the next connection form the socket
     *
     * @return EBTcpSocket* The socket. Otherwise nullptr
     */
    virtual EBObjectPointer<EBTcpSocket> nextConnection()
    {
        EB_PROFILE_FUNC();

        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        SOCKET connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli), &len);
        if (connfd >= 0)
        {
            EBObjectPointer<EBTcpSocket> socket = this->createObject<EBTcpSocket>(connfd, cli);
            socket->startThread();
            return socket;
        }

        return nullptr;
    }

private:
    bool bound;
    std::thread* thread;
    std::list< EBObjectPointer<EBTcpSocket> > clients;

    EB_SLOT(disconnected)
    {
        EB_PROFILE_FUNC();

        EBCpp::EBObjectPointer<EBCpp::EBTcpSocket> socket = sender->cast<EBCpp::EBTcpSocket>();
        EBCpp::EBObjectPointer<EBCpp::EBTcpSocket> toDelete(nullptr);

        for (auto c : clients)
        {
            if( c.get() == socket.get() )
            {
                toDelete = c;
            }
        }

        if( toDelete.get() != nullptr )
        {
            clients.remove(toDelete);
        }
    }

    void acceptConnections()
    {
        EB_PROFILE_FUNC();
        EB_LOG_DEBUG("TCP Server startet!");

        EBUtils::setThreadName("EBSslServer #" + std::to_string(socketId));

        while (bound)
        {
            EBObjectPointer<EBTcpSocket> socket = nextConnection();
            if (socket != nullptr)
            {
                socket->disconnected.connect(this, &EBTcpServer::disconnected);
                clients.push_back(socket);
                EB_EMIT_WITH_ARGS(newConnection, socket);
            }
        }
    }
};

} // namespace EBCpp