/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten (Tropby)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
#include "../socket/tcp/EBTcpSocket.hpp"
#include "EBHttpRequest.hpp"

namespace EBCpp
{

template <class socket = EBTcpSocket>
class EBHttpClient : public EBObject< EBHttpClient<> >
{
public:
    EBHttpClient() : protocol("HTTP/1.0"), isFinished(true)
    {
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

    EB_SIGNAL(finished);

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

    EB_SLOT(tcpConnected)
    {
        headerReceived = false;
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
        tcpSocket.readReady.disconnect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.disconnect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.disconnect(this, &EBHttpClient::tcpDisconnected);

        isFinished = true;
        EB_EMIT(finished);
    }

    EB_SLOT(tcpReadReady)
    {
        if (receivePayload.size() >= receiveSize && receiveSize != -1 && headerReceived)
            return;

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
                    break;
                }
                else
                {
                    if(line.contains(":"))
                    {
                        EBString key = line.mid(0, line.indexOf(":")).trim().toLower();
                        EBString value = line.mid(line.indexOf(":")+1).trim();

                        receiveHeader[key] = value;
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
                    tcpSocket.close();
                    return;
                }
            }
        }
    }
};

} // namespace EBCpp
