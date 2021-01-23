#pragma once


#include <string>

#include "socket/tcp/EBTcpHeader.hpp"

#ifdef __WIN32__
    #include <windows.h>  
#endif

namespace EBCpp
{

class EBUtils
{
public:    
    static std::string hostnameToIp(std::string hostname)
    {
        struct addrinfo hints, *servinfo, *p;
        struct sockaddr_in *h;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(hostname.c_str(), "http", &hints, &servinfo)) != 0)
        {
            return "";
        }

        // loop through all the results and connect to the first we can
        char ip[64];
        for (p = servinfo; p != NULL; p = p->ai_next)
        {
            h = (struct sockaddr_in*) p->ai_addr;
            strcpy(ip, inet_ntoa(h->sin_addr));
        }

        freeaddrinfo(servinfo); // all done with this structure
        return ip;

    }

    static void setThreadName(DWORD dwThreadID, const char* threadName) 
    {  
        pthread_setname_np(pthread_self(), "My thread name");
    }        
} ;

}

