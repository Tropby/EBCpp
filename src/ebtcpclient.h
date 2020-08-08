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


#ifndef DEP_EBCPP_EBTCPCLIENT_H_
#define DEP_EBCPP_EBTCPCLIENT_H_

#include "ebtcpsocket.h"
#include "ebsemaphore.h"

namespace EBCpp
{

class EBTcpClient: public EBTcpSocket, public std::enable_shared_from_this<EBTcpClient>
{
public:
	EBTcpClient(int socketId = -1, bool connected = false);
	virtual ~EBTcpClient();

    EB_SIGNAL(connected, std::shared_ptr<EBTcpClient>);
    EB_SIGNAL(disconnected, std::shared_ptr<EBTcpClient>);
    EB_SIGNAL(readReady, std::shared_ptr<EBTcpClient>);

	void connectToHost(std::string ip, uint16_t port);
	void connectToHost(uint32_t ip, uint16_t port);

	virtual void write(std::string data);
	virtual void write(char * data, int size );
	virtual std::vector<uint8_t> read();
	virtual std::string readString();

protected:
	virtual bool connectRaw();
	virtual bool readRaw();
	virtual bool runRaw();

private:
    EBSemaphore processConnect;
};

}

#endif /* DEP_EBCPP_EBTCPCLIENT_H_ */
