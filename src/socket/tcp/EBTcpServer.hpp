#pragma once

#include <cinttypes>
#include <thread>
#include <memory>
#include <functional>

#include "../../EBObject.hpp"
#include "../../EBEvent.hpp"

#include "EBTcpHeader.hpp"

namespace EBCpp
{

class EBTcpServer : public EBObject
{
public: 
    EBTcpServer() :
        EBObject(),
        socketId(-1),
        bound(false),
        thread(nullptr)
    {
        
    }

    virtual bool bind( uint16_t port )
    {
#ifdef __WIN32__
        WORD versionWanted = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(versionWanted, &wsaData);
#endif

	    socketId = socket( AF_INET, SOCK_STREAM, 0);

    	struct sockaddr_in servaddr;

    	memset(&servaddr, 0, sizeof(servaddr));

	    // assign IP, PORT
	    servaddr.sin_family = AF_INET;
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	    servaddr.sin_port = htons(port);

	    if ((::bind(socketId, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr))) != 0)
	    {
    		return false;
    	}
    	if ((::listen(socketId, 5)) != 0)
    	{
		    return false;
    	}

	    bound = true;
	    thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBTcpServer::acceptConnections, this)));
    	return true;
    }

    void unbind()
    {
        bound = false;
        ::close(socketId);

        if (thread)
        {
            thread->join();
        }
    }    

    EB_SIGNAL( newConnection );

private:
    SOCKET socketId;
    bool bound;
    std::unique_ptr<std::thread> thread;
    std::list< std::unique_ptr< EBTcpServerSocket > > clients;

    void acceptConnections()
    {
        while( bound )
        {
            struct sockaddr_in cli;
            socklen_t len = sizeof(cli);

            SOCKET connfd = ::accept(socketId, reinterpret_cast<sockaddr*>(&cli), &len);
		    if (connfd >= 0)
            {
                EB_EMIT( newConnection );

                //std::shared_ptr<EBTcpServerSocket> socket = std::make_shared<EBTcpServerSocket>(connfd);
                //socket->start();
                //newConnection.emit(socket);
            }
        }
    }

};

}