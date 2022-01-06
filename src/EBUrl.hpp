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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBEvent.hpp"
#include "EBObject.hpp"
#include "EBString.hpp"

namespace EBCpp
{

class EBUrl : public EBObject<EBUrl>
{
public:
    EBUrl(const char* url) : valid(false), port(0)
    {
        setUrl(url);
    }

    EBUrl(const std::string& url) : valid(false), port(0)
    {
        setUrl(url);
    }

    EBUrl(const EBString& url) : valid(false), port(0)
    {
        setUrl(url);
    }

    ~EBUrl()
    {
    }

    bool setUrl(const EBString& url)
    {
        protocol = url.mid(0, url.indexOf("://"));

        EBString rest = url.mid(url.indexOf("://") + 3);

        int32_t p1 = rest.indexOf(":");
        int32_t p2 = rest.indexOf("/");

        if (p1 > p2 || p1 == -1)
        {
            host = rest.mid(0, p2);
            rest = rest.mid(p2 + 1);
            port = -1;

            if (protocol == "https")
                port = 433;
            else if (protocol == "http")
                port = 80;
            else if (protocol == "ftp")
                port = 21;

            /// TODO: Add more standard ports. In a list? Maybe defines/static?
        }
        else
        {
            host = rest.mid(0, p1);
            rest = rest.mid(p1 + 1);

            p2 = rest.indexOf("/");
            port = rest.mid(0, p2).toInt();

            rest = rest.mid(p2 + 1);
        }

        p1 = rest.indexOf("?");
        if (p1 >= 0)
        {
            path = rest.mid(0, p1);
            rest = rest.mid(p1 + 1);

            do
            {
                int index = rest.indexOf("&");

                EBString str;
                if (index >= 0)
                {
                    str = rest.mid(0, index);
                    rest = rest.mid(index + 1);
                }
                else
                {
                    str = rest;
                    rest = "";
                }

                index = str.indexOf("=");

                EBString key, value;
                key = str.mid(0, index);
                value = str.mid(index + 1);

                args[key] = value;

            } while (rest.length() > 0);
        }
        else
        {
            path = rest;
        }

        return true;
    }

    EBString toString() const
    {
        EBString argList;
        for (auto arg : args)
        {
            argList += arg.first + "=" + arg.second + "&";
        }

        return protocol + "://" + host + ":" + std::to_string(port) + getQuery();
    }

    bool isValid() const
    {
        return valid;
    }

    void setProtocol(const EBString& protocol)
    {
        this->protocol = protocol;
    }

    void setHost(const EBString& host)
    {
        this->host = host;
    }

    void setPort(const uint16_t port)
    {
        this->port = port;
    }

    void setPath(const EBString& path)
    {
        this->path = path;
    }

    void setArg(const EBString& key, const EBString& value)
    {
        this->args[key] = value;
    }

    void unsetArg(const EBString& key)
    {
        this->args.erase(key);
    }

    void clearArgs()
    {
        this->args.clear();
    }

    const EBString& getProtocol() const
    {
        return protocol;
    }

    const EBString& getHost() const
    {
        return host;
    }

    int32_t getPort() const
    {
        return port;
    }

    const EBString& getPath() const
    {
        return path;
    }

    const EBString& getArg(const EBString& key) 
    {
        return args[key];
    }

    const std::map<EBString, EBString>& getArgList() const
    {
        return args;
    }

    const EBString getQuery() const
    {
        EBString argList;
        for (auto arg : args)
        {
            argList += arg.first + "=" + arg.second + "&";
        }

        return EBString("/") + path + "?" + argList;
    }

private:
    bool valid;

    EBString protocol;
    EBString host;
    int32_t port;
    EBString path;
    std::map<EBString, EBString> args;
};

} // namespace EBCpp

std::ostream& operator<<(std::ostream& os, const EBCpp::EBUrl& url)
{
    return os << url.toString();
}
