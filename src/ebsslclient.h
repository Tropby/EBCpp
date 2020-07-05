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


#ifndef DEP_EBCPP_EBSSLCLIENT_H_
#define DEP_EBCPP_EBSSLCLIENT_H_

#include "ebconfig.h"

#ifdef EB_OPENSSL

#include "ebtcpclient.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace EBCpp{

class EBSSLClient : public EBTcpClient
{
public:
	EBSSLClient();
	virtual ~EBSSLClient();

    virtual void write( std::string data );

protected:
    SSL *ssl;
    int sslSock;

	virtual bool readRaw();
	virtual bool connectRaw();

private:
	void logSslErrors();
};

}

#endif

#endif /* DEP_EBCPP_EBSSLCLIENT_H_ */
