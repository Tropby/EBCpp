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
 *  Created on: Sep 20, 2020
 *      Author: Carsten (Tropby)
 */

#include "ebhttpsserver.h"

#ifdef EB_OPENSSL

namespace EBCpp
{

EBHTTPSServer::EBHTTPSServer()
{
	server.newConnection.connect( std::bind( &EBHTTPSServer::newConnection, this, std::placeholders::_1 ) );
}

EBHTTPSServer::~EBHTTPSServer()
{

}

bool EBHTTPSServer::bind(uint16_t port)
{
	return server.bind(port);
}

void EBHTTPSServer::unbind()
{
	server.unbind();
}

} /* namespace EBCpp */

#endif
