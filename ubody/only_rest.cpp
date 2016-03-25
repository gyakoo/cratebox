//
//
#include <stdio.h>
#include <conio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <restserv.hpp>

/// http://arunranga.com/examples/access-control/credentialedRequest.html
/// three.js github webgl_geometries.html example.

class MyCallbacks : public RestServerCallbacksJSON
{
public:
	~MyCallbacks()
	{
		printf("destructor\n");
	}

	virtual void onClientDataReceived(RestServerClientConnectionJSON& client) override
	{
		RestServerClientConnectionJSON::Response resp;
		/*resp.setStatusCode(200, "OK");
		resp.getHeaders().set("Content-Type", "text/plain");
		resp.addToBody("Hello from the application server.");
		*/
    resp.addToBody("hello");
		client.sendResponse(resp);
	}
};

//
//
// Main console entry point
//
int main()
{
	RestServerJSON<2> server(std::make_shared<MyCallbacks>());
	if ( server.init(8080) != 0 )
		return 1;

	printf( "listening...\n");
	while (true)
	{
		server.stepListen();
	}
	server.quit();
	return 0;
}

/*


HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
Sec-WebSocket-Protocol: chat

*/