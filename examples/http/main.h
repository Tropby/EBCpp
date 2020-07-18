
#include "../../src/ebhttpserver.h"
#include "../../src/ebeventloop.h"

class HTTPTest
{
public:
	HTTPTest() : server( 44554 )
	{
		server.newRequest.connect(std::bind(&HTTPTest::newRequest, this, std::placeholders::_1 ));
	}

	~HTTPTest()
	{

	}

private:

	void newRequest( std::shared_ptr<EBCpp::EBHTTPRequest> request )
	{
		std::cout << "Request Received " << request->getRequestHeader().getPath() << std::endl;

		std::string data ="";
		for( int i = 0; i < 1500; i++ )
		{
			data += std::to_string(i) + "<br />";
		}

		request->sendReply("<html><head></head><body>" + data + "</body></html>");
	}

	EBCpp::EBHTTPServer server;
};

int mainHttpTest()
{
	try
	{
		HTTPTest tt;
		return EBCpp::EBEventLoop::getMainLoop().exec();
	}
	catch( ... )
	{
		std::cout << "ERROR" << std::endl;
		return 1;
	}
}

