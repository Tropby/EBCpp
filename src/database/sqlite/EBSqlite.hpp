#pragma once

#pragma once

#include "../../EBObject.hpp"
#include "../../EBString.hpp"
#include "../../EBMap.hpp"
#include "../../EBUrl.hpp"

#include "../EBDatabase.hpp"
#include "../../profile/EBLogger.hpp"

#include <sqlite3.h>

namespace EBCpp {

class EBSqlite : public EBDatabase
{
public:
    virtual bool connect( EBString host, int port, EBString database, EBString username, EBString password, EBMap<EBString, EBString> arguments)
    {
        EB_LOG_INFO( "SQLite Version: " << sqlite3_libversion() );


        EB_LOG_INFO("SQLite conneciton data: " << host << ":" << port << "?" << database << "  username: " << username << "  password: " << password);       
        for( auto kp : arguments )
        {
            EB_LOG_INFO("Argument: " << kp.first << " = " << kp.second);
        }
        return true;
    }

private:

};

}