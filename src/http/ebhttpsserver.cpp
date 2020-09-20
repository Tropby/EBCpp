/*
 * ebhttpsserver.cpp
 *
 *  Created on: 20.09.2020
 *      Author: tropb
 */

#include "ebhttpsserver.h"

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
