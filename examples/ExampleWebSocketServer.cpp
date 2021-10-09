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
#include "../src/EBFile.hpp"
#include "../src/http/EBHttpServer.hpp"
#include "../src/websocket/EBWebSocket.hpp"
#include "../src/socket/tcp/EBTcpServer.hpp"
#include "../src/socket/tcp/EBTcpSocket.hpp"

/**
 * @brief Example to show the function of the TCP server
 *
 */
class ExampleWebSocketServer : public EBCpp::EBObject<ExampleWebSocketServer>
{
public:
    /**
     * @brief Construct a new Example Tcp Server object
     *
     */
    ExampleWebSocketServer() 
    {
        server.newRequest.connect(this, &ExampleWebSocketServer::requestReady);
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
        if( request->getPath().compare("/echo") == 0 )
        {
            EB_LOG_DEBUG("Client connected to the WebsocketServer!");

            // Upgrade the http request to an websocket connection!
            sockets.push_back(createObject<EBCpp::EBWebSocket>(request));
            sockets.back()->textFrameReceived.connect(this, &ExampleWebSocketServer::textFrameReceived);
            sockets.back()->binaryFrameReceived.connect(this, &ExampleWebSocketServer::binaryFrameReceived);
            sockets.back()->disconnected.connect(this, &ExampleWebSocketServer::disconnected);
        }
        else
        {
            // Return the example file for websockets
            EBCpp::EBFile file;
            file.setFileName("../examples/websocketTest.html");
            if( file.open(EBCpp::EBFile::DIRECTION::READ_ONLY) )
            {
                request->sendReply(file.readAll());
            }
            else
            {
                request->sendReply("can not open example file!");
            }
        }
    }

    EB_SLOT_WITH_ARGS(textFrameReceived, std::string text)
    {
        EB_LOG_DEBUG(text);
        for( auto socket : sockets )
        {
            socket->sendTextFrame(text);
        }
    }

    /**
     * @brief Binary frame received
     *        Send the binary data to all clients.
     * 
     * @param data The binary data that was received
     */
    EB_SLOT_WITH_ARGS(binaryFrameReceived, std::vector<uint8_t> data )
    {
        EB_LOG_DEBUG("Binary Data received [" << data.size() << "]");
        for (auto socket : sockets)
        {
            socket->sendBinaryFrame(data);
        }
    }

    EB_SLOT(disconnected)
    {
        EB_LOG_DEBUG("Client disconnected!");
        EBCpp::EBObjectPointer<EBCpp::EBWebSocket> s = sender->cast<EBCpp::EBWebSocket>();
        for( auto socket : sockets )
        {
            if( socket == s )
            {
                sockets.remove(socket);
                break;
            }
        }
    }

private:
    EBCpp::EBHttpServer server;
    EBCpp::EBTcpServer tcpServer;

    std::list< EBCpp::EBObjectPointer<EBCpp::EBWebSocket> > sockets;
};

int main()
{
    ExampleWebSocketServer exampleHttpServer;
    EBCpp::EBEventLoop::getInstance()->exec();
}
