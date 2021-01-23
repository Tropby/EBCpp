
#include <iostream>
#include <functional>
#include <string>

#include "EBApplication.hpp"
#include "EBEvent.hpp"
#include "EBEventLoop.hpp"
#include "EBTimer.hpp"
#include "socket/tcp/EBTcpSocket.hpp"

#include "socket/tcp/EBTcpSocket.hpp"

class Test1 : public EBCpp::EBObject
{
public:
    Test1() : EBObject()
    {
        socket.connected.connect( *this, &Test1::connected );
        socket.disconnected.connect( *this, &Test1::disconnected );
        socket.error.connect( *this, &Test1::error );
        socket.readReady.connect( *this, &Test1::readReady );

        socket.setFileName("tcp://tropbyhome.zakdyn.de:443");
        socket.open(EBCpp::EBIODevice::READ_WRITE);    
    }

    EB_SLOT( connected )
    {
        std::cout << "connected" << std::endl;
    }

    EB_SLOT( disconnected )
    {
        std::cout << "disconnected" << std::endl;
        EBCpp::EBEventLoop::getInstance().exit();
    }

    EB_SLOT( readReady )
    {
        char buffer[1024];
        EBCpp::EBTcpSocket* socket = static_cast<EBCpp::EBTcpSocket*>(sender);

        int nbytes = socket->read(buffer, 1024);
        std::string s(buffer, nbytes);

        std::cout << "read Ready: " << s << std::endl;

        socket->close();
    }

    EB_SLOT_WITH_ARGS( error, std::string message )
    {
        std::cout << "ERROR: " << message << std::endl;
    }

private:
    EBCpp::EBTcpSocket socket;

};


int main()
{
    Test1 test1;
    EBCpp::EBEventLoop::getInstance().exec();

}
