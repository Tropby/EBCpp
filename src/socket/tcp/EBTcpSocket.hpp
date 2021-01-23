#pragma once

#include <atomic>

#include "../../EBIODevice.hpp"
#include "../../EBException.hpp"
#include "../../EBUtils.hpp"
#include "../../EBSemaphore.hpp"

#include "EBTcpHeader.hpp"

namespace EBCpp
{

class EBTcpSocket : public EBIODevice
{
public: 
    EBTcpSocket() : 
        EBIODevice(),
        connectionState(false),
        thread(nullptr)
    {
        static bool inited = false;
        if (!inited)
        {
            #ifdef __WIN32__
                    WORD versionWanted = MAKEWORD(1, 1);
                    WSADATA wsaData;
                    WSAStartup(versionWanted, &wsaData);
            #endif
            inited = true;
        }
        
    }

    virtual ~EBTcpSocket()
    {
        if( thread )
        {
            this->close();
            thread->join();
        }
    }

    virtual bool open( EBIODevice::DIRECTION direction )
    {
        
        if( direction != READ_WRITE )
            EB_EXCEPTION( "Can not open a tcp socket read only or write only." );

        // Check if hostname can be used for this connection
        std::string host = getFileName();

        if( host.substr(0, 6).compare("tcp://") )
            EB_EXCEPTION( "EBTcpSocket needs a tcp://{hostname}:{port} filename to connect to a host." );

        host = host.substr(6);

        if( host.find(':') == std::string::npos )
            EB_EXCEPTION( "EBTcpSocket needs a tcp://{hostname}:{port} filename to connect to a host." );

        int32_t port = std::stoi( host.substr( host.find(':') + 1 ) );
        host = host.substr( 0, host.find(':'));

        if( port > 0xFFFF || port < 0 )
            EB_EXCEPTION( "EBTcpSocket port rage (0 - 65535)." );

        // Get ip address from host name otherwise treat host as ip address
        std::string hostIp = EBUtils::hostnameToIp(host);
        if (!hostIp.empty())
            host = hostIp;

        // convert ip address string to ip address
        #ifdef __WIN32__    
            memset(&address,0,sizeof(SOCKADDR_IN)); 
            address.sin_family=AF_INET;
            address.sin_port=htons(port); 
            address.sin_addr.s_addr=inet_addr(host.c_str()); 
        #else
            inet_pton(AF_INET, host.c_str(), &address);
        #endif

        thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBTcpSocket::run, this)));
    }

    virtual bool isOpened() 
    {
        return connectionState;
    }

    virtual bool close()
    {
        if (isOpened())
        {
    #ifdef __WIN32__
            ::shutdown(socketId, SD_BOTH);
    #else
            ::shutdown(socketId, SHUT_RDWR);
    #endif
            ::close(socketId);
        }        
        else
        {
            EB_EXCEPTION( "Can not close a unopend connection!" );
        }
    }

    virtual int write( char * data, int length )
    {
        send(socketId, data, length, 0);
    }

    virtual int write( std::string data )
    {
        send(socketId, data.c_str(), data.length(), 0);
    }

    virtual int read( char * data, int maxLength ) 
    {
        int size = this->data.size() < maxLength ? this->data.size() : maxLength;

        for( int i = 0; i < size; i++ )
        {
            *data = this->data.front();
            data++;
            this->data.pop_front();
        }
    }
    
    virtual std::string readLine() 
    {        
        bool found = (std::find(data.begin(), data.end(), '\n') != data.end());
        if( found )
        {
            std::string result;
            char c = 0;
            while( c != '\n' )
            {
                c = this->data.front();
                result += c;
                this->data.pop_front();
            }
        }
        else
        {
            EB_EXCEPTION( "Can not read line. No \\n found in received data!" );
        }
    }

    EB_SIGNAL_WITH_ARGS( error, std::string );
    EB_SIGNAL( connected );
    EB_SIGNAL( disconnected );
    EB_SIGNAL( readReady );

private:
    bool connectionState;
    SOCKADDR_IN address;
    std::list<char> data;
    std::unique_ptr<std::thread> thread;
    SOCKET socketId;

    /**
     * This method is used for the thread.
     * Handle the connection, read events.
     **/
    void run()
    {
        // Try to get a new socket
        socketId = ::socket( AF_INET, SOCK_STREAM, 0);
        if( socketId == -1 )
        {
            EB_EMIT_WITH_ARGS( error, "Can not create socket!" );
            return;
        }

        // Try to connect to host
        int descriptor = ::connect(socketId, reinterpret_cast<sockaddr*>(&address), sizeof(address));
        if (descriptor == -1)
        {
            EB_EMIT_WITH_ARGS( error, "Can not connect to address!" );
            return;
        }
        connectionState = true;
        EB_EMIT(connected);

        // Read until the end of life
        char buffer[1024];
        int nbytes;
        while( true )
        {
            // Read next block of data
            nbytes = ::recv(socketId, buffer, sizeof(buffer), 0);

            switch (nbytes)
            {
            // Discriptor disconnected
            case 0:
                EB_EMIT(disconnected);
                return;

            case -1:
                EB_EMIT_WITH_ARGS(error, "Error while reading from socket!");
                return;

            default:
                std::vector<uint8_t> b(buffer, buffer + nbytes);
                data.insert(data.end(), std::begin(b), std::end(b));
                EB_EMIT(readReady);
            }
        }

        return;
    }
};

}