#pragma once

#include "../EBObject.hpp"
#include "../EBString.hpp"
#include "../EBMap.hpp"
#include "../EBUrl.hpp"

#include "EBStatement.hpp"

namespace EBCpp {

class EBDatabase : public EBObject<EBDatabase>
{
public:
    virtual bool connect( EBUrl url ) 
    {
        EBString host;
        int port;
        EBString database;
        EBString username;
        EBString password;
        EBMap<EBString, EBString> arguments;

        host = url.getHost();
        port = url.getPort();
        database = url.getPath();
        username = url.getArg("username");
        password = url.getArg("password");

        EBList<EBString> keyList = url.getArgKeyList();
        for( auto key : keyList )
        {
            arguments[ key.get() ] = url.getArg(key.get());
        }

        return connect(host, port, database, username, password, arguments);
    }

    virtual bool connect( EBString host, int port, EBString database, EBString username, EBString password, EBMap<EBString, EBString> arguments) = 0;

    virtual void close() = 0;

    virtual EBPtr<EBStatement> prepare( EBString query ) = 0;
    virtual bool exec( EBString query ) = 0;
    
private:

};

}