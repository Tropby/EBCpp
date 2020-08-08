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
 *  Created on: Jul 15, 2020
 *      Author: Carsten (Tropby)
 */

#include "ebhttpserver.h"

using namespace EBCpp;

EBHTTPServer::EBHTTPServer()
{
	server.newConnection.connect( std::bind( &EBHTTPServer::newConnection, this, std::placeholders::_1 ) );
}

EBHTTPServer::~EBHTTPServer()
{

}

void EBCpp::EBHTTPServer::newConnection(std::shared_ptr< EBTcpServerSocket > client)
{
	std::shared_ptr< EBHTTPRequest > request = std::make_shared<EBHTTPRequest>( client );
	request->ready.connect(std::bind( &EBHTTPServer::requestReady, this, std::placeholders::_1 ));
	requests.push_back( request );
	request->start();
}

bool EBCpp::EBHTTPServer::bind(uint16_t port)
{
	return server.bind(port);
}

void EBCpp::EBHTTPServer::requestReady(std::shared_ptr<EBHTTPRequest> request)
{
	requests.remove(request);
	newRequest.emit(request);
}
