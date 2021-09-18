#pragma once

#include <iostream>
#include <map>
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
class EBHttpRequest : public EBObject<EBHttpRequest>
{
public:
    /**
     * @brief Construct a new EBHttpRequest object
     *
     * @param parent Parent of the http request
     */
    EBHttpRequest() : tcpSocket(nullptr), headerFinished(false), responseCode(200), firstLine(true), readFinished(false)
    {
    }

    ~EBHttpRequest()
    {
        tcpSocket = nullptr;
    }

    /**
     * @brief Set the socket the request is performed on
     *
     * @param tcpSocket the socket
     */
    void setSocket(EBObjectPointer<EBTcpSocket> tcpSocket)
    {
        this->tcpSocket = tcpSocket;
        tcpSocket->readReady.connect(this, &EBHttpRequest::readReady);
        readReady(tcpSocket->template cast<EBObject<EBObjectBase>>());
    }

    /**
     * @brief Get the Data that was received by the EBHttpRequest
     * 
     * @return std::vector<char> the data
     */
    std::vector<char> getData()
    {
        return data;
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
        tcpSocket->write("Content-Length: " + std::to_string(data.size()) + "\r\n");
        tcpSocket->write("\r\n");
        tcpSocket->write(data);
        tcpSocket->close();
        tcpSocket->readReady.disconnect(this, &EBHttpRequest::readReady);
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
     * @brief Get the Post Parameter
     * 
     * @param key The key of the parameter that should be returned
     * @return std::string the value of the parameter
     */
    std::string getPostParameter(std::string key)
    {
        return postParameter[key];
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
    EBObjectPointer<EBTcpSocket> tcpSocket;
    bool headerFinished;
    int contentSize = -1;

    EBHttpHeader requestHeader;
    EBHttpHeader replyHeader;

    std::string requestMethod;
    std::string requestPath;
    std::string requestProtocol;

    int responseCode;
    bool firstLine;
    bool readFinished;

    std::vector<char> data;

    std::map<std::string, std::string> postParameter;

    void protocolError()
    {
        tcpSocket->write("Protocol Error!");
        tcpSocket->close();
        EB_EXCEPTION("Protocol Error!");
    }

    EB_SLOT(readReady)
    {
        EB_PROFILE_FUNC();

        if (readFinished)
            return;

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
                        extractMethodProtocolAndPath(line);
                    }
                    else if (line.size() == 0)
                    {
                        headerFinished = true;
                        break;
                    }
                    else
                    {
                        requestHeader.processLine(line);
                        if (contentSize == -1 && requestHeader.contains("content-length"))
                        {
                            std::string len = requestHeader.getValue("content-length");
                            contentSize = std::stoi(len);
                        }
                    }
                }
            }
        }
        catch (EBException& ex)
        {
            std::cerr << ex.what() << '\n';
        }

        if (contentSize > 0)
        {
            while (!tcpSocket->atEnd())
            {
                char buffer[1024];
                int bytesRead = tcpSocket->read(buffer, 1024);
                for (int i = 0; i < bytesRead; i++)
                    data.push_back(buffer[i]);
                contentSize -= bytesRead;
            }
        }

        if (headerFinished && contentSize <= 0)
        {
            if (requestHeader.contains("content-type"))
            {
                if (EBUtils::toLower(requestHeader.getValue("content-type"))
                    .compare("application/x-www-form-urlencoded") == 0)
                {
                    processPOSTParameters();
                }
            }
            readFinished = true;
            EB_EMIT(ready);
        }
    }

    void extractMethodProtocolAndPath(std::string line)
    {
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

    void processPOSTParameters()
    {
        std::vector<char> v = getData();
        std::string param(v.begin(), v.end());
        std::stringstream ss(param);

        std::string token;
        while (std::getline(ss, token, '&'))
        {
            std::string key = EBUtils::trim(token.substr(0, token.find('=')));
            std::string value = EBUtils::trim(token.substr(token.find('=') + 1));

            key = EBUtils::urlDecode(key);
            value = EBUtils::urlDecode(value);

            postParameter[key] = value;
        }
    }
};

} // namespace EBCpp