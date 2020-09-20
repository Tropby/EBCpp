/*
 * ebhttpsserver.h
 *
 *  Created on: 20.09.2020
 *      Author: tropb
 */

#ifndef SRC_HTTP_EBHTTPSSERVER_H_
#define SRC_HTTP_EBHTTPSSERVER_H_

#include "ebhttpserver.h"
#include "../socket/tcp/ssl/ebsslserver.h"

namespace EBCpp
{

class EBHTTPSServer : public EBHTTPServer
{
public:
	EBHTTPSServer();
	virtual ~EBHTTPSServer();

	virtual bool bind(uint16_t port);
	virtual void unbind();

private:
	EBSSLServer server;
};

} /* namespace EBCpp */

#endif /* SRC_HTTP_EBHTTPSSERVER_H_ */
