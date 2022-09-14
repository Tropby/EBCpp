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
 *  Created on: 2022-09-10
 *      Author: Carsten (Tropby)
 */

#pragma once

#include <memory>
#include <thread>

#ifdef __WIN32__
#include <winsock2.h>
#endif

#ifdef __WIN32__
#include <icmpapi.h>
#include <ipexport.h>
#include <iphlpapi.h>
#else
#error NOT IMPLEMENTED FOR UNIX
#endif

#include "../../EBObject.hpp"
#include "../../EBUrl.hpp"
#include "../../EBUtils.hpp"

namespace EBCpp
{

class EBICMP : public EBObject<EBICMP>
{
public:
    EBICMP(EBString destination) : destination(destination), running(false)
    {
    }

    EBICMP() : destination(""), running(false)
    {
    }

    void setDestination(EBUrl destination)
    {
        this->destination = destination;
    }

    EBUrl getDestination()
    {
        return this->destination;
    }

    struct ICMP_RESULT
    {
        EBString host;
        int status;
        int roundtrip;
        EBString resultString;
    };

    bool ping()
    {
        if( running )
            return false;

        running = true;
        if( thread )
        {
            if( thread->joinable() )
            {
                thread->join();
            }
        }

        thread = std::make_shared<std::thread>(&EBICMP::run, this);
        return true;
    }

    EBString getLastError() const
    {
        return lastError;
    }

    EB_SIGNAL_WITH_ARGS(finished, ICMP_RESULT);
    EB_SIGNAL(error);

private:
    std::shared_ptr<std::thread> thread;
    EBUrl destination;
    EBString lastError;
    bool running;

    void run()
    {
        HANDLE hIcmpFile;
        unsigned long ipaddr = INADDR_NONE;
        DWORD dwRetVal = 0;
        char SendData[32] = "Data Buffer";
        LPVOID ReplyBuffer = NULL;
        DWORD ReplySize = 0;

        ipaddr = inet_addr(destination.getHost().dataPtr());

        hIcmpFile = IcmpCreateFile();
        if (hIcmpFile == INVALID_HANDLE_VALUE)
        {
            running = false;
            lastError = "Can not create ICMP Handle!";
            EB_EMIT(error);
            return;
        }

        ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
        ReplyBuffer = (VOID*)malloc(ReplySize);
        if (ReplyBuffer == NULL)
        {
            running = false;
            lastError = "Can not allocate reply buffer!";
            EB_EMIT(error);
            return;
        }

        dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 0, ReplyBuffer, ReplySize, 5000);
        if (dwRetVal != 0)
        {
            PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
            struct in_addr ReplyAddr;
            ReplyAddr.S_un.S_addr = pEchoReply->Address;

            ICMP_RESULT result;
            result.host = destination.getHost();
            result.roundtrip = pEchoReply->RoundTripTime;
            result.status = pEchoReply->Status;

            WCHAR buffer[128];
            char str[128];
            long unsigned int size = 128;
            GetIpErrorString(result.status, buffer, &size);
            wcstombs(str, buffer, 128);
            result.resultString = str;

            running = false;
            EB_EMIT_WITH_ARGS(finished, result);
        }
        else
        {
            lastError = EBString() + "Timeout! [Code: " + EBUtils::intToStr(GetLastError()) + "]";
            running = false;
            EB_EMIT(error);
        }
    }
};
} // namespace EBCpp
