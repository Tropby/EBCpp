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
#include "../src/http/EBHttpServer.hpp"
#include "../src/socket/tcp/EBTcpServer.hpp"
#include "../src/socket/tcp/EBTcpSocket.hpp"

/**
 * @brief Example to show the function of the TCP server
 *
 */
class ExampleHttpServer : public EBCpp::EBObject<ExampleHttpServer>
{
public:
    /**
     * @brief Construct a new Example Tcp Server object
     *
     */
    ExampleHttpServer() 
    {
        server.newRequest.connect(this, &ExampleHttpServer::requestReady);
        auto test = &tcpServer;
        server.setTcpServer(test);

        if (tcpServer.bind(8958, "127.0.0.1"))
        {
            std::cout << "Http server now bound on 8958. You can open the test page at http://127.0.0.1:8958/"
                      << std::endl;
        }
        else
        {
            std::cout << "Can not bind to port 8958" << std::endl;
        }
    }

    /**
     * @brief EB_SLOT requestReady
     *
     * This slot will called if a request is made and ready
     * to be abalyzed
     *
     * @param sender The sender object (Http Server)
     * @param request The request object ready to be answered
     */
    EB_SLOT_WITH_ARGS(requestReady, EBCpp::EBObjectPointer<EBCpp::EBHttpRequest> request)
    {
        static int count = 0;
        request->sendReply("<html><head><title>Hello World!</title>"
                           "<meta http-equiv='refresh' content='1' />"
                            "</head>"
                            "<body>"
                            "Hello World!<br />"
                            "Called: " +
                           std::to_string(++count) +
                           "</body>"
                           "</html>");
    }

private:
    EBCpp::EBHttpServer server;
    EBCpp::EBTcpServer tcpServer;
};

int main()
{
    ExampleHttpServer exampleHttpServer;
    EBCpp::EBEventLoop::getInstance()->exec();
}
