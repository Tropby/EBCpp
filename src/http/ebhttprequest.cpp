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

#include <string>
#include <vector>

#include "ebhttprequest.h"

using namespace EBCpp;

EBCpp::EBHTTPRequest::EBHTTPRequest(std::shared_ptr<EBTcpClient> &socket) :
		readHeader(true), socket(socket)
{
	socket->readReady.connect(
			std::bind(&EBHTTPRequest::readReady, this, std::placeholders::_1));
}

EBHTTPRequest::~EBHTTPRequest()
{
	socket->close();
}

void EBCpp::EBHTTPRequest::sendReply(std::string data)
{
	socket->write(replyHeader.getHeader());
	socket->write("\r\n"); // Empty line after header
	socket->write(data);
}

void EBCpp::EBHTTPRequest::sendReply(std::vector<uint8_t> data)
{
	socket->write(replyHeader.getHeader());
	socket->write("\r\n"); // Empty line after header
	socket->write(reinterpret_cast<char*>(&data[0]), data.size());
}

void EBCpp::EBHTTPRequest::readReady(EBTcpClient *client)
{
	std::cout << "readReady" << std::endl;

	// read all data read
	std::vector<uint8_t> d = client->read();
	data.insert(data.end(), d.begin(), d.end());

	if (readHeader)
	{
		for (unsigned int i = 3; i < data.size(); i++)
		{
			char c1 = data[i - 3];
			char c2 = data[i - 2];
			char c3 = data[i - 1];
			char c4 = data[i - 0];

			if (c1 == '\r' && c2 == '\n' && c3 == '\r' && c4 == '\n')
			{
				std::cout << "Line received" << std::endl;
				std::string header = std::string(
						reinterpret_cast<char*>(&data[0]), i);
				requestHeader.parse(header);

				data = std::vector<uint8_t>(data.begin() + i, data.end());

				readHeader = false;
				break;
			}

		}
	}
	else
	{

	}
}
