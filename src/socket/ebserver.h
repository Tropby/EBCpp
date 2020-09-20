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
 *  Created on: Sep 22, 2020
 *      Author: Carsten (Tropby)
 */

#ifndef SRC_SOCKET_EBSERVER_H_
#define SRC_SOCKET_EBSERVER_H_

#include <memory>
#include "ebserversocket.h"

namespace EBCpp
{

class EBServer
{
public:
	EBServer(){};
	virtual ~EBServer(){};

	EB_SIGNAL( newConnection, std::shared_ptr< EBServerSocket > );

	virtual bool bind(uint16_t port) = 0;
	virtual void unbind() = 0;

protected:
	virtual void acceptConnections() = 0;

	std::list< std::shared_ptr< EBServerSocket > > clients;
};


} /* namespace EBCpp */

#endif /* SRC_SOCKET_EBSERVER_H_ */
