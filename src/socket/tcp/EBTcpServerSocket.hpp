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

#pragma once

#include "../../EBException.hpp"
#include "EBTcpHeader.hpp"
#include "EBTcpSocket.hpp"

namespace EBCpp
{

/**
 * @brief Class that handles a tcp socket for the EBTcpServer
 * 
 */
class EBTcpServerSocket : public EBTcpSocket
{
public:
    /**
     * @brief Construct a new EBTcpServerSocket object
     * 
     * @param parent EBTcpServer that have created the object
     * @param socketId socket id of the tcp connection
     * @param client client informations
     */
    EBTcpServerSocket( EBObject* parent, SOCKET socketId, struct sockaddr_in client ) : 
        EBTcpSocket(parent)
    {
        this->socketId = socketId;
        this->address = client;
        startThread();
    }

    /**
     * @brief Overwritten method (do not use)
     * 
     * The server handels the accept connection. Therefore the socket handler 
     * can not open a new connection. The method is overwritten to inform
     * the user that you can not open a server socket connection after it
     * is closed.
     * 
     * @param direction (do not use)
     * @return true never
     * @return false allways
     */
    virtual bool open( DIRECTION direction )
    {
        EB_EXCEPTION("Can not open. Server sockets are opened by default!");
        return false;
    }
};

}
