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

#ifndef DEP_EBCPP_EBTCPSERVER_H_
#define DEP_EBCPP_EBTCPSERVER_H_

#include <atomic>
#include <thread>

#include "../ebserver.h"

#include "tcp.h"
#include "ebtcpserversocket.h"

namespace EBCpp
{

class EBTcpServer : public EBServer
{
public:
	EBTcpServer();
	virtual ~EBTcpServer();

	bool bind(uint16_t port);
	void unbind();

protected:
	void acceptConnections();

private:
	std::unique_ptr<std::thread> thread;
	std::atomic<bool> deleted;
	SOCKET socketId;

};

}

#endif /* DEP_EBCPP_EBTCPSERVER_H_ */
