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

#define EB_OPEN_SSL

#include "../src/EBString.hpp"
#include "../src/EBEvent.hpp"
#include "../src/EBEventLoop.hpp"
#include "../src/EBIODevice.hpp"
#include "../src/EBTimer.hpp"
#include "../src/socket/tcp/ssl/EBSslSocket.hpp"

/**
 * @brief Example to show the function of a tcp client
 *
 */
class ExampleSSLClient : public EBCpp::EBObject < ExampleSSLClient>
{
public:
    /**
     * @brief Construct a new Example Tcp Client object
     *
     * @param parent Parent of this object
     */
    ExampleSSLClient() 
    {
        socket.connected.connect(this, &ExampleSSLClient::connected);
        socket.disconnected.connect(this, &ExampleSSLClient::disconnected);
        socket.error.connect(this, &ExampleSSLClient::error);
        socket.readReady.connect(this, &ExampleSSLClient::readReady);

        socket.setFileName("tcp://google.com:443");
        socket.open(EBCpp::EBIODevice<EBCpp::EBTcpSocket>::READ_WRITE);
    }

    /**
     * @brief EB_SLOT connected
     *
     * Called if a client connection is established
     *
     * @param sender The sender object
     */
    EB_SLOT(connected)
    {
        std::cout << "connected" << std::endl;
        EBCpp::EBObjectPointer<EBCpp::EBTcpSocket> socket = sender->cast<EBCpp::EBTcpSocket>();

        socket->write(EBCpp::EBString("GET / HTTP/1.0\r\nhost: www.google.com\r\n\r\n"));
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
        std::cout << "disconnected" << std::endl;
        EBCpp::EBEventLoop::getInstance()->exit();
    }

    /**
     * @brief EB_SLOT readReady
     *
     * Called if data is available from the client
     *
     * @param sender The sender object
     */
    EB_SLOT(readReady)
    {
        char buffer[1024];
        EBCpp::EBObjectPointer<EBCpp::EBTcpSocket> socket = sender->cast<EBCpp::EBTcpSocket>();

        int nbytes = socket->read(buffer, 1024);
        std::string s(buffer, nbytes);

        std::cout << s;
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
    EBCpp::EBSslSocket socket;
};

/**
 * @brief The main of the tcp client example
 *
 * @return int Exit code (0)
 */
int main()
{
    ExampleSSLClient exampleTcpClient;
    EBCpp::EBEventLoop::getInstance()->exec();
}
