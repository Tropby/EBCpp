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

#include <cinttypes>
#include <functional>
#include <memory>
#include <thread>

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
class EBTcpServer : public EBObject
{
public:
    /**
     * @brief Construct a new EBTcpServer object
     *
     * @param parent Parent object of the tcp server
     */
    EBTcpServer(EBObject* parent) : EBObject(parent), socketId(-1), bound(false), thread(nullptr)
    {
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
#ifdef __WIN32__
        WORD versionWanted = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(versionWanted, &wsaData);
#endif

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
        thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBTcpServer::acceptConnections, this)));
        return true;
    }

    /**
     * @brief Unbind the server socket from the port
     */
    void unbind()
    {
        bound = false;
        ::close(socketId);

        if (thread)
        {
            thread->join();
        }
    }

    /**
     * @brief EB_SIGNAL newConnection
     *
     * This signal will be called for each tcp connection that will be established
     */
    EB_SIGNAL_WITH_ARGS(newConnection, EBTcpSocket*);

protected:
    //! current socket id
    SOCKET socketId;

    /**
     * @brief Get the next connection form the socket
     *
     * @return EBTcpSocket* The socket. Otherwise nullptr
     */
    virtual EBTcpSocket* nextConnection()
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        SOCKET connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli), &len);
        if (connfd >= 0)
        {
            EBTcpSocket* socket = new EBTcpSocket(this, connfd, cli);
            socket->startThread();
            return socket;
        }

        return nullptr;
    }

private:
    bool bound;
    std::unique_ptr<std::thread> thread;
    std::list<EBTcpSocket*> clients;

    void acceptConnections()
    {
        EBUtils::setThreadName("EBSslServer #" + std::to_string(socketId));

        while (bound)
        {
            EBTcpSocket* socket = nextConnection();
            if (socket != nullptr)
            {
                clients.push_back(socket);
                EB_EMIT_WITH_ARGS(newConnection, socket);
            }
        }
    }
};

} // namespace EBCpp