#pragma once

#include <sstream>

#include "../EBObject.hpp"
#include "../EBEvent.hpp"
#include "../socket/tcp/EBTcpServerSocket.hpp"

#include "EBHttpHeader.hpp"

namespace EBCpp
{

/**
 * @brief Handles a http request
 */
class EBHttpRequest : public EBObject
{
public:
    /**
     * @brief Construct a new EBHttpRequest object
     * 
     * @param tcpSocket Socket that will be used for the http request
     * @param parent Parent of the http request
     */
    EBHttpRequest( EBTcpSocket * tcpSocket, EBObject * parent ) : 
        EBObject(parent),
        tcpSocket(tcpSocket),
        headerFinished(false),
        requestHeader(this),
        replyHeader(this),
        responseCode(200),
        firstLine(true)
    {
        tcpSocket->readReady.connect(*this, &EBHttpRequest::readReady);
    }

    /**
     * @brief Sends a reply to the http client and closes the connection
     * 
     * @param data Data to send
     */
    void sendReply( std::string data )
    {
        tcpSocket->write("HTTP/1.0 " + std::to_string(responseCode) + " Okay\r\n");
        tcpSocket->write(replyHeader.getHeader());
        tcpSocket->write("\r\n");
        tcpSocket->write(data);
        tcpSocket->close();
        delete tcpSocket;
    }

    /**
     * @brief Get the Request Header object
     * 
     * @return EBHttpHeader& 
     */
    EBHttpHeader& getRequestHeader() 
    {
        return requestHeader;
    }

    /**
     * @brief Get the Reply Header object
     * 
     * @return EBHttpHeader& 
     */
    EBHttpHeader& getReplyHeader() 
    {
        return replyHeader;
    }

    /**
     * @brief Get the request method string
     * 
     * @return std::string 
     */
    std::string getRequestMethod()
    {
        return requestMethod;
    }

    /**
     * @brief Get the path as string
     * 
     * @return std::string 
     */
    std::string getPath()
    {
        return requestPath;
    }

    /**
     * @brief Set the response code 
     * 
     * @param code 
     */
    void setResponseCode( int code )
    {
        responseCode = code;
    }

    /**
     * @brief EB_SIGNAL ready
     * 
     * Signal is emitted if the request receive has been finished
     */
    EB_SIGNAL(ready);

private:
    EBTcpSocket * tcpSocket;
    bool headerFinished;
    int contentSize = -1;

    EBHttpHeader requestHeader;
    EBHttpHeader replyHeader;

    std::string requestMethod;
    std::string requestPath;
    std::string requestProtocol;

    int responseCode;
    bool firstLine;

    void protocolError()
    {

    }

    EB_SLOT(readReady)
    {
        try
        {
            if(!headerFinished)
            {
                while( tcpSocket->canReadLine() )
                {
                    std::string line = EBUtils::trim(tcpSocket->readLine());
                    
                    if( firstLine )
                    {
                        firstLine = false;                        
                        std::istringstream f(line);
                        std::string s;

                        if( !getline(f, s, ' ') ) protocolError();
                        requestMethod = s;
                        
                        if( !getline(f, s, ' ') ) protocolError();
                        requestPath = s;    

                        if( !getline(f, s, ' ') ) protocolError();
                        requestProtocol = s;
                    }
                    else if( line.size() == 0 )
                        headerFinished = true;
                    else
                        requestHeader.processLine(line);
                }
            }
        }
        catch(EBException& ex)
        {
            std::cerr << ex.what() << '\n';
        }
        
        if( headerFinished && contentSize <= 0 )
        {
            EB_EMIT( ready );
        }
    }
};

}