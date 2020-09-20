
#include "../../src/ebeventloop.h"
#include "../../src/http/ebhttpsserver.h"

class HTTPSTest
{
public:
	HTTPSTest() : server()
	{
		server.newRequest.connect(std::bind(&HTTPSTest::newRequest, this, std::placeholders::_1 ));
		server.bind(44544);
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

