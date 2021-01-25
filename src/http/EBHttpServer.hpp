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
 *  Created on: 2021-01-24
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "../EBObject.hpp"
#include "../EBEvent.hpp"
#include "EBHttpRequest.hpp"
#include "../socket/tcp/EBTcpServer.hpp"

namespace EBCpp
{

/**
 * @brief Implementation of a http server
 * 
 */
class EBHttpServer : public EBObject
{
public:
    /**
     * @brief Construct a new EBHttpServer object
     * 
     * @param parent Parent of the http server object
     */
    EBHttpServer(EBObject * parent) : 
        EBObject(parent),
        tcpServer(nullptr)        
    {
    }

    virtual ~EBHttpServer()
    {
        if( tcpServer )
            delete tcpServer;
    }

    /**
     * @brief Set the Tcp Server object
     * 
     * @tparam T 
     * @param server 
     * @return true 
     * @return false 
     */
    bool setTcpServer(EBTcpServer* server)
    {
        if( server == nullptr )
            return false;
        tcpServer = server;
        tcpServer->newConnection.connect( *this, &EBHttpServer::newConnection );      
        return true;
    }

    /**
     * @brief EB_SIGNAL newRequest
     * 
     * Will be emitted if a new http request is ready
     * 
     */
    EB_SIGNAL_WITH_ARGS( newRequest, EBHttpRequest* );

private:
    EBTcpServer* tcpServer;
    std::list< EBHttpRequest* > requests;

    /**
     * @brief EB_SLOT newConnection
     * 
     * Will called by the EBTcpServer if a new connection is 
     * available
     * 
     * @param sender Object that has emmited the signal
     * @param socket New tcp socket
     */
    EB_SLOT_WITH_ARGS(newConnection, EBTcpSocket * socket)
    {
        EBHttpRequest* request = new EBHttpRequest( socket, this );
        request->ready.connect(*this, &EBHttpServer::requestReady);
        requests.push_back(request);
    }

    /**
     * @brief EB_SLOT requestReady
     * 
     * Will be called if a request is ready
     * 
     */
    EB_SLOT(requestReady)
    {
        EBHttpRequest* request = static_cast<EBHttpRequest*>(sender);
        EB_EMIT_WITH_ARGS( newRequest, request );
    }

};

}
