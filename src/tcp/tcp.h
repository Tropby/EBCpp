

#ifdef __WIN32__
	//#define socklen_t int
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <io.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>	//hostent
	#include <arpa/inet.h>
	#define SOCKET int32_t
#endif
