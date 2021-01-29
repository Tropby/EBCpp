#pragma once

#include <sstream>

#include "../EBEvent.hpp"
#include "../EBObject.hpp"
#include "../socket/tcp/EBTcpSocket.hpp"

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
     * @param parent Parent of the http request
     */
    EBHttpRequest(EBObject* parent) :
        EBObject(parent), tcpSocket(tcpSocket), headerFinished(false), requestHeader(this), replyHeader(this),
        responseCode(200), firstLine(true)
    {
        std::cout << "EBHttpRequest" << std::endl;
    }

    ~EBHttpRequest()
    {
        std::cout << "~EBHttpRequest" << std::endl;
        delete tcpSocket;
    }

    /**
     * @brief Set the socket the request is performed on
     * 
     * @param tcpSocket the socket
     */
    void setSocket(EBTcpSocket* tcpSocket)
    {
        this->tcpSocket = tcpSocket;
        tcpSocket->readReady.connect(*this, &EBHttpRequest::readReady);
        readReady(tcpSocket);
    }

    /**
     * @brief Sends a reply to the http client and closes the connection
     *
     * @param data Data to send
     */
    void sendReply(std::string data)
    {
        tcpSocket->write("HTTP/1.0 " + std::to_string(responseCode) + " Okay\r\n");
        tcpSocket->write(replyHeader.getHeader());
        tcpSocket->write("\r\n");
        tcpSocket->write(data);
        tcpSocket->close(); 
        EB_EMIT(finished);
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
    void setResponseCode(int code)
    {
        responseCode = code;
    }

    /**
     * @brief EB_SIGNAL ready
     *
     * Signal is emitted if the request receive has been finished
     */
    EB_SIGNAL(ready);

    /**
     * @brief EB_SIGNAL finished
     * 
     * This signal will be emitted if the request is finished and the
     * reply is send to the client.
     */
    EB_SIGNAL(finished);

private:
    EBTcpSocket* tcpSocket;
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
            if (!headerFinished)
            {
                while (tcpSocket->canReadLine())
                {
                    std::string line = EBUtils::trim(tcpSocket->readLine());

                    if (firstLine)
                    {
                        firstLine = false;
                        std::istringstream f(line);
                        std::string s;

                        if (!getline(f, s, ' '))
                            protocolError();
                        requestMethod = s;

                        if (!getline(f, s, ' '))
                            protocolError();
                        requestPath = s;

                        if (!getline(f, s, ' '))
                            protocolError();
                        requestProtocol = s;
                    }
                    else if (line.size() == 0)
                        headerFinished = true;
                    else
                        requestHeader.processLine(line);
                }
            }
        }
        catch (EBException& ex)
        {
            std::cerr << ex.what() << '\n';
        }

        if (headerFinished && contentSize <= 0)
        {
            EB_EMIT(ready);
        }
    }
};

} // namespace EBCpp