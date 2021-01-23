#pragma once

#include "../../EBObject.hpp"
#include "../../EBEvent.hpp"

#include "EBTcpHeader.hpp"

namespace EBCpp
{

class EBTcpServerSocket : public EBObject
{
public:
    EBTcpServerSocket( SOCKET socketId, struct sockaddr_in client ) : 
        EBObject(),
        socketId(socketId),
        client(client)
    {

    }

    EB_SIGNAL( disconnected );
    EB_SIGNAL( readReady );
    EB_SIGNAL( error );

private:
    SOCKET socketId;
    struct sockaddr_in client;
};

}
