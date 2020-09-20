/*
 * ebsslserver.h
 *
 *  Created on: 20.09.2020
 *      Author: tropb
 */

#ifndef SRC_TCP_SSL_EBSSLSERVER_H_
#define SRC_TCP_SSL_EBSSLSERVER_H_

#include "../../../ebconfig.h"

#ifdef EB_OPENSSL

#include <atomic>

#include "../tcp.h"
#include "ebsslserversocket.h"
#include "../../ebserver.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace EBCpp
{

class EBSSLServer : public EBServer
{
public:
	EBSSLServer();
	virtual ~EBSSLServer();

	bool bind(uint16_t port);
	void unbind();

protected:
	void acceptConnections();

private:
	std::unique_ptr<std::thread> thread;
	std::atomic<bool> deleted;
	SOCKET socketId;
	SSL_CTX *ctx;
};

} /* namespace EBCpp */

#endif

#endif /* SRC_TCP_SSL_EBSSLSERVER_H_ */
