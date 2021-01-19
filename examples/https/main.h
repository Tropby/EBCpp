
#include "../../src/ebeventloop.h"
#include "../../src/http/ebhttpsserver.h"

class HTTPSTest
{
public:
	HTTPSTest() : server()
	{
		server.newRequest.connect(std::bind(&HTTPSTest::newRequest, this, std::placeholders::_1 ));
		if( server.bind(44544) )
		{
			std::cout << "Server started on port 44544. Use https://127.0.0.1:44544/ for tests." << std::endl;
		}
		else
		{
			std::cout << "Error binding port 44544!" << std::endl;
			exit;
		}
	}

	~HTTPSTest()
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

		request->sendReply("<html><head><meta http-equiv=\"refresh\" content=\"1\"></head><body>" + data + "</body></html>");
	}

	EBCpp::EBHTTPSServer server;
};

int mainHttpsTest()
{
	std::cout << "START HTTP SERVER" << std::endl;
	try
	{
		HTTPSTest tt;
		return EBCpp::EBEventLoop::getMainLoop().exec();
	}
	catch( std::exception & ex )
	{
		std::cout << "EXCEPTION " << ex.what() << std::endl;
	}
	std::cout << "MAIN ENDED" << std::endl;
	return 0;
}

