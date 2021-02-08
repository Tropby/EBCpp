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

#include <functional>
#include <iostream>
#include <string>

#include "../src/EBEvent.hpp"
#include "../src/EBEventLoop.hpp"
#include "../src/EBTimer.hpp"
#include "../src/socket/tcp/EBTcpServer.hpp"
#include "../src/socket/tcp/EBTcpSocket.hpp"

/**
 * @brief Example to show the function of the TCP server
 *
 */
class ExampleTcpServer : public EBCpp::EBObject
{
public:
    /**
     * @brief Construct a new Example Tcp Server object
     *
     */
    ExampleTcpServer() : EBCpp::EBObject(nullptr), server(this)
    {
        server.newConnection.connect(*this, &ExampleTcpServer::newConnection);
        if (server.bind(8958))
        {
            std::cout << "Tcp server now bound on 8958" << std::endl;
        }
        else
        {
            std::cout << "Can not bind to port 8958" << std::endl;
        }
    }

    /**
     * @brief EB_SLOT newConnection
     *
     * Called if a new client socket is available
     *
     * @param sender The sender object
     * @param socket The new client socket
     */
    EB_SLOT_WITH_ARGS(newConnection, EBCpp::EBTcpSocket* socket)
    {
        std::cout << "new connection!" << std::endl;

        socket->disconnected.connect(*this, &ExampleTcpServer::disconnected);
        socket->readReady.connect(*this, &ExampleTcpServer::readReady);
        socket->error.connect(*this, &ExampleTcpServer::error);

        socket->write("Hallo :)\n");
    }

    /**
     * @brief EB_SLOT disconnected
     *
     * Called if a client disconnects
     *
     * @param sender The sender object
     */
    EB_SLOT(disconnected)
    {
        EBCpp::EBTcpSocket* socket = static_cast<EBCpp::EBTcpSocket*>(sender);
        std::cout << "disconnected" << std::endl;
        delete socket;
    }

    /**
     * @brief EB_SLOT readReady
     *
     * Called if data is available from a client
     *
     * @param sender The sender object
     */
    EB_SLOT(readReady)
    {
        char buffer[1024];
        EBCpp::EBTcpSocket* socket = static_cast<EBCpp::EBTcpSocket*>(sender);

        int nbytes = socket->read(buffer, 1024);
        std::string s(buffer, nbytes);

        std::cout << "read Ready: " << s << std::endl;

        if (socket->isOpened())
            socket->close();
    }

    /**
     * @brief EB_SLOT error
     *
     * Is called by a client socket if an error occured.
     *
     * @param sender The sender object
     * @param message Error message
     */
    EB_SLOT_WITH_ARGS(error, std::string message)
    {
        std::cout << "ERROR: " << message << std::endl;
    }

private:
    EBCpp::EBTcpServer server;
};

int main()
{
    ExampleTcpServer exampleTcpServer;
    EBCpp::EBEventLoop::getInstance().exec();
}
