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
#include "EBList.hpp"

namespace EBCpp
{

class EBUrl : public EBObject<EBUrl>
{
public:
    EBUrl() : port(0)
    {

    }

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
        EBString rest = url;

        int32_t schemaEnd = url.indexOf(":");
        if( schemaEnd > 0 )
        {
            protocol = url.mid( 0, schemaEnd ).toUpper();
            rest = url.mid( schemaEnd + 1 );            
            
            if( protocol == "HTTP")
                this->port = 80;
            else if( protocol == "HTTPS" )
                this->port = 443;
            else if( protocol == "FTP" )
                this->port = 21;
            else if( protocol == "SSH" )
                this->port = 22;
            else
                this->port = 0;
        }
        else
        {
            this->port = 0;
            this->protocol = "UNKNOWN";
        }

        // check if authority is available
        if( rest.startsWith("//") )
        {
            rest = rest.mid(2);
            EBString authority = rest;
            if( authority.indexOf("/") >= 0 )
            {
                authority = authority.mid( 0, authority.indexOf("/") );
                rest = rest.mid(rest.indexOf("/"));
            }
            else
            {
                rest = "";
            }

            // userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
            if( authority.indexOf("@") > 0 )
            {
                EBString userinfo = authority.mid(0, authority.indexOf("@"));
                authority = authority.mid(authority.indexOf("@") + 1);
                if( userinfo.indexOf(":") )
                {
                    this->username = userinfo.mid(0, userinfo.indexOf(":"));
                    this->password = userinfo.mid(userinfo.indexOf(":")+1);
                }
            }

            host = authority;

            // Check for port
            if( host.indexOf(":") > 0 )
            {
                EBString port = rest.mid(host.indexOf(":")+1);
                this->port = port.toInt();
            }
        }

        this->path = rest;
        if(this->path.contains("?"))
        {
            EBString query = this->path.mid(this->path.indexOf("?")+1);
            this->path = this->path.mid(0, this->path.indexOf("?"));

            if( query.contains("#") )
            {
                this->fragment = query.mid( query.indexOf("#")+1 );
                query = query.mid( 0, query.indexOf("#") );
            }
            else
            {
                this->fragment = "";
            }

            while( !query.empty() )
            {
                EBString q;
                if( query.contains("&" ) ) 
                {
                    q = query.mid( 0, query.indexOf("&") );
                    query= query.mid( query.indexOf("&")+1 );
                }
                else
                {
                    q = query;
                    query = "";
                }

                ARGUMENT a;
                if(q.contains("="))
                {
                    a.key = q.mid(0,q.indexOf("="));
                    a.value = q.mid(q.indexOf("=") + 1);
                }
                else
                {
                    a.key = "";
                    a.value = q;
                }
                arguments.append(a);
            }
        }

        EB_LOG_DEBUG(
            "Protocol: " << this->protocol << " | " <<
            "Username: " << this->username << " | " <<
            "Password: " << this->password << " | " <<
            "Host: " << this->host << " | " <<
            "Port: " << this->port << " | " << 
            "Path: " << this->path << " | "
        );

        return true;
    }

    EBString toString() const
    {
        EBString auth;

        if( !username.empty() )
            auth = username;
        if( !password.empty() )
        {
            auth += ":";
            auth += password;
        }
        if( !auth.empty() )
            auth += "@";

        return protocol.toLower() + "://" + auth + host + (port ? ":" + std::to_string(port) : "" ) + getQuery() + fragment;
    }

    bool isValid() const
    {
        return valid;
    }

    void setProtocol(const EBString& protocol)
    {
        this->protocol = protocol.toUpper();
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

    void setUsername(const EBString& username)
    {
        this->username = username;
    }

    void setPassword(const EBString& password)
    {
        this->password = password;
    }        

    void setFragment(const EBString& fragment)
    {
        this->fragment = fragment;
    }

    void setArg(const EBString& key, const EBString& value)
    {
        bool found = false;
        for( int i = 0; i < arguments.getSize(); i++ )
        {
            ARGUMENT a = arguments.get(i);
            if( a.key == key )
            {
                a.value = value;
                arguments.removeAt(i);
                arguments.append(a);
                return;
            }
        }
        
        ARGUMENT a;
        a.key = key;
        a.value = value;
        arguments.append(a);
    }

    void unsetArg(const EBString& key)
    {
        for( int i = 0; i < arguments.getSize(); i++ )
        {
            if( arguments.get(i).key == key )
            {
                this->arguments.removeAt(i);
                return;
            }
        }
    }

    void clearArgs()
    {
        this->arguments.clear();
    }

    const EBString getProtocol() const
    {
        return protocol.toUpper();
    }

    const EBString getHost() const
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

    const EBString& getUsername() const
    {
        return username;
    }

    const EBString& getPassword() const
    {
        return password;
    }    

    const EBString& getFragment() const
    {
        return fragment;
    }

    const EBString getArg(const EBString& key) 
    {
        for( auto & a : arguments )
        {
            if( a.get().key == key )
            {
                return a.get().value;
            }
        }
        return "";
    }

    const int getArgCount() const
    {
        return arguments.getSize();
    }

    const EBList<EBString> getArgKeyList() const
    {
        EBList<EBString> result;
        for( auto & a : arguments )
        {
            result.append(a.get().key);
        }
        return result;
    }

    const EBString getQuery() const
    {
        EBString argList;
        for (auto& arg : arguments)
        {
            if( !arg.get().key.empty() )
                argList += arg.get().key + "=" + arg.get().value + "&";
            else
                argList += arg.get().value + "&";
        }

        EBString path;
        path = this->path;
        if( !host.empty() && !path.startsWith("/") )
            path = EBString("/") + path;

        return path + (!argList.empty() ? EBString("?") + argList.mid(0, argList.length() - 1) : "");
    }

private:
    bool valid;

    EBString protocol;
    EBString host;
    EBString username;
    EBString password;
    int32_t port;
    EBString path;
    EBString fragment;

    typedef struct {
        EBString key;
        EBString value;
    } ARGUMENT;
    EBList<ARGUMENT> arguments;
};

} // namespace EBCpp

std::ostream& operator<<(std::ostream& os, const EBCpp::EBUrl& url)
{
    return os << url.toString();
}
