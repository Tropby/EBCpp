/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
 *  Created on: 2021-01-24
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "../EBEvent.hpp"
#include "../EBObject.hpp"
#include "../EBUrl.hpp"
#include "../EBString.hpp"
#include "../EBMap.hpp"
#include "../EBTimer.hpp"
#include "../socket/tcp/EBTcpSocket.hpp"
#include "EBHttpRequest.hpp"

/// TODO: Make timeout configurable in 100ms
#define HTTP_TIMEOUT 10

namespace EBCpp
{

template <class socket = EBTcpSocket>
class EBHttpClient : public EBObject< EBHttpClient<> >
{
public:
    EBHttpClient() : protocol("HTTP/1.0"), isFinished(true)
    {
        timer.timeout.connect(this, &EBHttpClient::timeout);
    }

    bool get(const EBString& host, uint16_t port, const EBString& path)
    {
        if( !isFinished )
        {
            return false;
        }
        isFinished = false;

        tcpSocket.readReady.connect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.connect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.connect(this, &EBHttpClient::tcpDisconnected);
        tcpSocket.error.connect(this, &EBHttpClient::tcpError);        

        sendHeader["user-agent"] = "EBCppHttpClient";
        sendHeader["accept"] = "*/*";
        sendHeader["connection"] = "close";

        this->path = path.toStdString();
        sendHeader["host"] = host.toStdString();
        method = "GET";
        headerReceived = false;

        receiveHeader.clear();
        receivePayload.clear();
        receiveSize = -1;

        tcpSocket.setFileName("tcp://" + host.toStdString() + ":" + std::to_string(port));

        timer.start(100);
        time = 0;

        if (!tcpSocket.open(EBTcpSocket::READ_WRITE))
            return false;
        return true;
    }

    bool post(const EBString& host, uint16_t port, const EBString& path, EBMultiMap<EBString, EBString>& arguments)
    {
        if( !isFinished )
        {
            return false;
        }
        isFinished = false;

        tcpSocket.readReady.connect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.connect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.connect(this, &EBHttpClient::tcpDisconnected);
        tcpSocket.error.connect(this, &EBHttpClient::tcpError);

        sendHeader["user-agent"] = "EBCppHttpClient";
        sendHeader["accept"] = "*/*";
        sendHeader["connection"] = "close";

        this->path = path.toStdString();
        sendHeader["host"] = host.toStdString();
        method = "POST";
        headerReceived = false;

        receiveHeader.clear();
        receivePayload.clear();
        receiveSize = -1;

        for( auto e : arguments )
        {
            sendPayload += e.first + "=" + e.second + "&";
        }

        tcpSocket.setFileName("tcp://" + host.toStdString() + ":" + std::to_string(port));

        timer.start(1000);
        time = 0;

        if (!tcpSocket.open(EBTcpSocket::READ_WRITE))
            return false;
        return true;
    }

    bool get(EBUrl url)
    {
        return get(url.getHost(), url.getPort(), url.getQuery());
    }

    bool post(EBUrl url, EBMultiMap<EBString, EBString>& arguments)
    {
        return post(url.getHost(), url.getPort(), url.getQuery(), arguments);
    }

    virtual ~EBHttpClient()
    {
        
    }

    std::vector<char> getResult()
    {
        return receivePayload;
    }

    EBString getResultString()
    {
        std::string s(receivePayload.begin(), receivePayload.end());
        return s;
    }

    bool isRequestFinished()
    {
        return isFinished;
    }

    bool isReceivingFinished()
    {
        return receivingFinished;
    }

    bool hasError()
    {
        return isError;
    }

    EBString getErrorMessage()
    {
        return errorMessage;
    }

    EB_SIGNAL(finished);
    EB_SIGNAL(error);

private:
    socket tcpSocket;
    std::string method;
    std::string path;
    std::string protocol;

    std::map<std::string, std::string> sendHeader;
    EBString sendPayload;

    EBMap<EBString, EBString> receiveHeader;
    std::vector<char> receivePayload;
    int64_t receiveSize;
    bool headerReceived;

    bool isFinished;
    bool receivingFinished;
    bool isError;
    bool receiveFirstLine;
    EBString errorMessage;

    EBTimer timer;
    int time;
    EB_SLOT(timeout)
    {
        time++;
        if( time >= HTTP_TIMEOUT )
        {
            if( tcpSocket.isOpened() )
            {
                isError = true;
                errorMessage = "Receiving timeout!";
                tcpSocket.close();
            }
            timer.stop();
        }
    }

    EB_SLOT_WITH_ARGS(tcpError, EBString errorMessage)
    {
        timer.stop();

        tcpSocket.readReady.disconnect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.disconnect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.disconnect(this, &EBHttpClient::tcpDisconnected);
        tcpSocket.error.disconnect(this, &EBHttpClient::tcpError);

        this->errorMessage = errorMessage;
        isError = true;
        isFinished = true;
        tcpSocket.close();
        EB_EMIT(error);
    }

    EB_SLOT(tcpConnected)
    {
        headerReceived = false;
        receivingFinished = false;
        isError = false;
        receiveFirstLine = true;

        receiveSize = -1;
        sendHeader["content-length"] = std::to_string(sendPayload.length());
        tcpSocket.write(method + " " + path + " " + protocol + "\r\n");
        for (std::pair<std::string, std::string> it : sendHeader)
        {
            tcpSocket.write(it.first + ": " + it.second + "\r\n");
        }
        tcpSocket.write(EBString("\r\n"));
        tcpSocket.write(sendPayload);
    }

    EB_SLOT(tcpDisconnected)
    {
        timer.stop();
        
        tcpSocket.readReady.disconnect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.disconnect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.disconnect(this, &EBHttpClient::tcpDisconnected);
        tcpSocket.error.disconnect(this, &EBHttpClient::tcpError);

        if(!isReceivingFinished() && !isError)
        {
            isError = true;
            errorMessage = "Disconnected before receiving finished!";
        }

        isFinished = true;

        if( isError )
        {
            EB_EMIT(error);
        }
        else
        {
            EB_EMIT(finished);
        }
    }

    EB_SLOT(tcpReadReady)
    {
        if (receivePayload.size() >= receiveSize && receiveSize != -1 && headerReceived)
        {
            tcpSocket.close();
            return;
        }

        // Reset timeout for long time connections
        time = 0;

        if (!headerReceived)
        {
            while (tcpSocket.canReadLine())
            {
                EBString line = tcpSocket.readLine();

                if (line.trim() == "")
                {
                    EBString contentLength = receiveHeader["content-length"];
                    if (contentLength != "")
                        receiveSize = contentLength.toInt();
                    headerReceived = true;
                    if( receiveSize <= 0 )
                    {
                        receivingFinished = true;
                    }

                    if( !receiveHeader["transfer-encoding"].empty() )
                    {
                        errorMessage = EBString("Transfer-Encoding \"") + receiveHeader["transfer-encoding"] + "\" not supported!";
                        isError = true;
                        tcpSocket.close();
                        return;
                    }

                    break;
                }
                else
                {
                    if( receiveFirstLine )
                    {
                        /// TODO: Add the method and Status check!
                    }
                    else if(line.contains(":"))
                    {
                        EBString key = line.mid(0, line.indexOf(":")).trim().toLower();
                        EBString value = line.mid(line.indexOf(":")+1).trim();

                        receiveHeader[key] = value;
                    }
                    else
                    {
                        errorMessage = EBString("Wrong header received! \"") + line.trim()+ "\" not supported!";
                        isError = true;
                        tcpSocket.close();
                        return;
                    }
                }
            }
        }

        if (headerReceived)
        {
            while (!tcpSocket.atEnd())
            {
                char buffer[1024];
                int size;
                size = tcpSocket.read(buffer, 1023);
                buffer[size] = 0x00;

                char* b = buffer;
                while (size > 0)
                {
                    receivePayload.push_back(*b);
                    b++;
                    size--;
                }

                if (receivePayload.size() >= receiveSize && receiveSize != -1)
                {
                    receivingFinished = true;
                    tcpSocket.close();
                    return;
                }
            }
        }
    }
};

} // namespace EBCpp
