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
#include "../src/socket/tcp/EBTcpSocket.hpp"

/**
 * @brief Example to show the function of a tcp client
 *
 */
class ExampleTcpClient : public EBCpp::EBObject<ExampleTcpClient>
{
public:
    /**
     * @brief Construct a new Example Tcp Client object
     *
     * @param parent Parent of this object
     */
    ExampleTcpClient() 
    {
        socket.connected.connect(this, &ExampleTcpClient::connected);
        socket.disconnected.connect(this, &ExampleTcpClient::disconnected);
        socket.error.connect(this, &ExampleTcpClient::error);
        socket.readReady.connect(this, &ExampleTcpClient::readReady);

        timer.timeout.connect(this, &ExampleTcpClient::timeout);
        
        reconnect();
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
        static int id = 0;
        std::cout << "connected" << std::endl;

        if( id % 2 == 0 )
        {
            socket.write(EBCpp::EBString("GET /cm?cmnd=status%208 HTTP/1.0\r\n\r\n"));
        }
        else
        {
            socket.write(EBCpp::EBString("GET /cm?cmnd=power0 HTTP/1.0\r\n\r\n"));
        }

        id++;
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
        EBCpp::EBObjectPointer<EBCpp::EBTcpSocket> socket = sender->cast<EBCpp::EBTcpSocket>();

        int nbytes;
        char buffer[1024];
        while( (nbytes = socket->read(buffer, 1024) ) > 0 )
        {
            std::string s(buffer, nbytes);
            std::cout << s;
        }

        std::cout << std::endl;
        std::cout << "disconnected" << std::endl;

        //EBCpp::EBEventLoop::getInstance()->exit();
        reconnect();
    }

    void reconnect()
    {
        timer.stop();
        timer.startSingleShot(2000);
        socket.setFileName("tcp://192.168.0.204:80");
        if( !socket.open(EBCpp::EBIODevice<EBCpp::EBTcpSocket>::READ_WRITE) )
        {
            std::cout << "Can not connect!" << std::endl;
        }
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
        // DO NOTHING. WAIT FOR DISCONNECTION TO READ ALL DATA
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
        reconnect();
    }

    EB_SLOT(timeout)
    {
        if( socket.isOpened() )
        {
            socket.close();
        }
    }

private:
    EBCpp::EBTcpSocket socket;
    EBCpp::EBTimer timer;
};

/**
 * @brief The main of the tcp client example
 *
 * @return int Exit code (0)
 */
int main()
{
    ExampleTcpClient exampleTcpClient;
    EBCpp::EBEventLoop::getInstance()->exec();
}
