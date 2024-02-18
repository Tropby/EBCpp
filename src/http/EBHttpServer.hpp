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
 *  Created on: 2021-01-24
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "../EBEvent.hpp"
#include "../EBObject.hpp"
#include "../socket/tcp/EBTcpServer.hpp"
#include "EBHttpRequest.hpp"

namespace EBCpp
{

/**
 * @brief Implementation of a http server
 *
 */
class EBHttpServer : public EBObject<EBHttpServer>
{
public:
    /**
     * @brief Construct a new EBHttpServer object
     *
     * @param parent Parent of the http server object
     */
    EBHttpServer() : tcpServer(nullptr)
    {
    }

    virtual ~EBHttpServer()
    {
    }

    /**
     * @brief Set the Tcp Server object
     *
     * @tparam T
     * @param server
     * @return true
     * @return false
     */
    bool setTcpServer(EBObjectPointer<EBTcpServer> server)
    {
        if (server == nullptr)
            return false;
        tcpServer = server;
        tcpServer->newConnection.connect(this, &EBHttpServer::newConnection);
        return true;
    }

    /**
     * @brief EB_SIGNAL newRequest
     *
     * Will be emitted if a new http request is ready
     *
     */
    EB_SIGNAL_WITH_ARGS(newRequest, EBCpp::EBObjectPointer<EBCpp::EBHttpRequest>);

private:
    EBObjectPointer<EBTcpServer> tcpServer;
    std::list<EBObjectPointer<EBHttpRequest>> requests;

    /**
     * @brief EB_SLOT newConnection
     *
     * Will called by the EBTcpServer if a new connection is
     * available
     *
     * @param sender Object that has emmited the signal
     * @param socket New tcp socket
     */
    EB_SLOT_WITH_ARGS(newConnection, EBObjectPointer<EBTcpSocket> socket)
    {
        EBObjectPointer<EBHttpRequest> request = createObject<EBHttpRequest>();
        request->ready.connect(this, &EBHttpServer::requestReady);
        request->finished.connect(this, &EBHttpServer::requestFinished);
        requests.push_back(request);
        request->setSocket(socket);
    }

    /**
     * @brief EB_SLOT requestReady
     *
     * Will be called if a request is ready
     *
     */
    EB_SLOT(requestReady)
    {
        EBCpp::EBObjectPointer<EBCpp::EBHttpRequest> request = sender->cast<EBCpp::EBHttpRequest>();
        EB_EMIT_WITH_ARGS(newRequest, request);
    }

    EB_SLOT(requestFinished)
    {
        EBCpp::EBObjectPointer<EBCpp::EBHttpRequest> request = sender->cast<EBCpp::EBHttpRequest>();

        for( auto & it : requests )
        {
            if (it.get() == request.get())
            {
                requests.remove(it);
                break;
            }
        }
    }
};

} // namespace EBCpp
