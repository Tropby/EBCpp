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
#include "../socket/tcp/EBTcpSocket.hpp"
#include "EBHttpRequest.hpp"
#include "../EBUrl.hpp"

namespace EBCpp
{

class EBHttpClient : public EBObject<EBHttpClient>
{
public:
    EBHttpClient() : protocol("HTTP/1.0")
    {
        tcpSocket.readReady.connect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.connect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.connect(this, &EBHttpClient::tcpDisconnected);

        sendHeader["user-agent"] = "EBCppHttpClient";
        sendHeader["accept"] = "*/*";
        sendHeader["connection"] = "close";
    }

    bool get(const EBString& host, uint16_t port, const EBString& path)
    {
        this->path = path.toStdString();
        sendHeader["host"] = host.toStdString();

        tcpSocket.setFileName("tcp://" + host.toStdString() + ":" + std::to_string(port));

        if (!tcpSocket.open(EBTcpSocket::READ_WRITE))
            return false;
        return true;
    }

    bool get(EBUrl url)
    {
        return get(url.getHost(), url.getPort(), url.getQuery());
    }

    virtual ~EBHttpClient()
    {
        
    }

    std::vector<char> getResult()
    {
        return receivePayload;
    }

    EB_SIGNAL(finished);

private:
    EBTcpSocket tcpSocket;
    std::string method;
    std::string path;
    std::string protocol;

    std::map<std::string, std::string> sendHeader;
    std::vector<char> sendPayload;

    std::map<std::string, std::string> receiveHeader;
    std::vector<char> receivePayload;
    int64_t receiveSize;
    bool headerReceived;

    EB_SLOT(tcpConnected)
    {
        headerReceived = false;
        receiveSize = -1;
        sendHeader["content-length"] = std::to_string(sendPayload.size());
        tcpSocket.write(method + " " + path + " " + protocol + "\r\n");
        for (std::pair<std::string, std::string> it : sendHeader)
        {
            tcpSocket.write(it.first + ": " + it.second + "\r\n");
        }
        tcpSocket.write("\r\n");
        tcpSocket.write(sendPayload.data(), sendPayload.size());
    }

    EB_SLOT(tcpDisconnected)
    {
        tcpSocket.readReady.disconnect(this, &EBHttpClient::tcpReadReady);
        tcpSocket.connected.disconnect(this, &EBHttpClient::tcpConnected);
        tcpSocket.disconnected.disconnect(this, &EBHttpClient::tcpDisconnected);

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
                std::string line = tcpSocket.readLine().toStdString();

                if (EBUtils::trim(line) == "")
                {
                    std::string contentLength = receiveHeader["content-length"];
                    if (contentLength != "")
                        receiveSize = std::atoi(contentLength.c_str());
                    headerReceived = true;
                    break;
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
