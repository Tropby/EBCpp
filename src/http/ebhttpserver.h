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

#ifndef SRC_HTTP_EBHTTPSERVER_H_
#define SRC_HTTP_EBHTTPSERVER_H_

#include "../ebevent.h"
#include "ebhttprequest.h"

namespace EBCpp
{

class EBHTTPServer
{
public:
	EBHTTPServer();
	virtual ~EBHTTPServer();

	EB_SIGNAL(newRequest, std::shared_ptr< EBHTTPRequest >);

	virtual bool bind(uint16_t port) = 0;
	virtual void unbind() = 0;

protected:
	EB_SLOT void requestReady(std::shared_ptr< EBHTTPRequest > request);

	EB_SLOT void newConnection(std::shared_ptr< EBServerSocket > client);

	std::list< std::shared_ptr< EBHTTPRequest > > requests;
};

} /* namespace EBCpp */

#endif /* SRC_HTTP_EBHTTPSERVER_H_ */
