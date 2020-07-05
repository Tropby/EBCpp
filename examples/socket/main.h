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
 *  Created on: Jul 3, 2020
 *      Author: Carsten (Tropby)
 */

#include <iostream>

#include "../../ebtcpclient.h"

using namespace std;
using namespace EBCpp;

class SocketTest
{
public:
	SocketTest()
	{
		socket.connected.connect( std::bind( &SocketTest::connected, this, std::placeholders::_1 ));
		socket.readReady.connect( std::bind( &SocketTest::readReady, this, std::placeholders::_1 ));
		socket.disconnected.connect( std::bind( &SocketTest::disconnected, this, std::placeholders::_1 ));

		socket.connectToHost("192.168.1.1", 80);
	}

private:
    EBTcpClient socket;

    void connected( EBTcpClient * socket )
    {

    }

    void readReady( EBTcpClient * socket )
    {

    }

    void disconnected( EBTcpClient * socket )
    {

    }

    void error(  EBTcpClient * socket, int err )
    {

    }


};

int mainSocketTest()
{
	SocketTest tt;

    return EBEventLoop::getMainLoop().finished();
}
