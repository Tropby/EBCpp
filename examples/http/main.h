
#include "../../src/ebhttpserver.h"
#include "../../src/ebeventloop.h"

class HTTPTest
{
public:
	HTTPTest() : server( 44554 )
	{
	}

	~HTTPTest()
	{

	}

private:

	EBCpp::EBHTTPServer server;
};

int mainHttpTest()
{
	HTTPTest tt;
	return EBCpp::EBEventLoop::getMainLoop().exec();
}

