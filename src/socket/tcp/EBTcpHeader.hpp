#pragma once

#ifdef __WIN32__
    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>
    #include <ws2def.h>
    #include <io.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>	//hostent
    #include <arpa/inet.h>
    #define SOCKET int32_t
#endif