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

#ifndef SRC_HTTP_EBHTTPREQUEST_H_
#define SRC_HTTP_EBHTTPREQUEST_H_

#include <memory>
#include <vector>

#include "../ebevent.h"
#include "ebhttpheader.h"
#include "../socket/tcp/ebtcpserversocket.h"
#include "../socket/tcp/ssl/ebsslserversocket.h"

namespace EBCpp
{

class EBHTTPRequest: public std::enable_shared_from_this<EBHTTPRequest >
{
public:
	EBHTTPRequest(std::shared_ptr<EBServerSocket> &socket) :
		std::enable_shared_from_this<EBHTTPRequest>(),
			readHeader(true), socket(socket), contentLength(0)
	{
		EB_DEBUG( socket->getSocketId() );
	}

	virtual ~EBHTTPRequest()
	{
		EB_DEBUG( socket->getSocketId() );
		socket->close();
	}


	EB_SIGNAL( ready, std::shared_ptr<EBHTTPRequest> );

	void sendReply(std::string data)
	{
		replyHeader.setValue("content-length", std::to_string(data.length()));

		socket->write(replyHeader.getHeader());
		socket->write("\r\n"); // Empty line after header
		socket->write(data);
		socket->close();
	}

	void sendReply(std::vector<uint8_t> data)
	{
		replyHeader.setValue("content-length", std::to_string(data.size()));

		socket->write(replyHeader.getHeader());
		socket->write("\r\n"); // Empty line after header
		socket->write(reinterpret_cast<char*>(&data[0]), data.size());
		socket->close();
	}

	const EBHTTPHeader& getRequestHeader() const
	{
		return requestHeader;
	}


	void start()
	{
		socket->readReady.connect(
				std::bind(&EBHTTPRequest::readReady, this, std::placeholders::_1));
		readReady(socket);
	}

private:
	EB_SLOT void readReady(std::shared_ptr<EBServerSocket> client)
	{
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
					std::string header = std::string(
							reinterpret_cast<char*>(&data[0]), i);
					requestHeader.parse(header);

					data = std::vector<uint8_t>(data.begin() + i, data.end());

					std::string cl = requestHeader.getValue("content-length");

					if( !cl.empty() )
						contentLength = std::stoi(cl);
					else
						contentLength = 0;


					readHeader = false;
					break;
				}

			}
		}

		if( !readHeader )
		{
			if( data.size() >= contentLength )
			{
				ready.emit( this->shared_from_this() );
			}
		}
	}

	std::vector<uint8_t> data;

	EBHTTPHeader requestHeader;
	EBHTTPHeader replyHeader;

	bool readHeader;
	std::shared_ptr<EBServerSocket> socket;
	unsigned int contentLength;
};

}

#endif /* SRC_HTTP_EBHTTPREQUEST_H_ */
