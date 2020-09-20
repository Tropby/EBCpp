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

#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "../ebevent.h"
#include "../socket/ebserversocket.h"
#include "ebhttpheader.h"

namespace EBCpp
{

class EBHTTPRequest: public std::enable_shared_from_this<EBHTTPRequest >
{
public:
	EBHTTPRequest(std::shared_ptr<EBServerSocket> &socket);

	virtual ~EBHTTPRequest();


	EB_SIGNAL( ready, std::shared_ptr<EBHTTPRequest> );

	void sendReply(std::string data);

	void sendReply(std::vector<uint8_t> data);

	const EBHTTPHeader& getRequestHeader() const;


	void start();

private:
	EB_SLOT void readReady(std::shared_ptr<EBServerSocket> client);

	std::vector<uint8_t> data;

	EBHTTPHeader requestHeader;
	EBHTTPHeader replyHeader;

	bool readHeader;
	std::shared_ptr<EBServerSocket> socket;
	unsigned int contentLength;
};

}

#endif /* SRC_HTTP_EBHTTPREQUEST_H_ */
