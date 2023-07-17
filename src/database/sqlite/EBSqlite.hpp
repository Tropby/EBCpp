#pragma once

#pragma once

#include "../../EBObject.hpp"
#include "../../EBString.hpp"
#include "../../EBMap.hpp"
#include "../../EBUrl.hpp"

#include "../EBDatabase.hpp"
#include "../../profile/EBLogger.hpp"

#include <sqlite3.h>
#include "EBSqliteStatement.hpp"

namespace EBCpp {

class EBSqlite : public EBDatabase
{
public:
    EBSqlite() : db(nullptr)
    {
        EB_LOG_INFO( "SQLite Version: " << sqlite3_libversion() );
    }

    ~EBSqlite()
    {
        if( db != nullptr )
        {
            sqlite3_close(db);
        }
    }

    bool connect( EBUrl url )
    {
        return EBDatabase::connect(url);
    }

    bool connect( EBString host, int port, EBString database, EBString username = "", EBString password = "", EBMap<EBString, EBString> arguments = EBMap<EBString, EBString>())
    {
        if( database.startsWith("/") )
        {
            database = database.mid(1);
        }
        int result = sqlite3_open_v2(database.dataPtr(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

        if( result )
        {
            EB_LOG_ERROR("Can't open database: " << sqlite3_errmsg(db));
            return false;
        }

        return true;
    }

    void close()
    {
        sqlite3_close(db);
    }

    EBPtr<EBStatement> prepare( EBString query )
    {
        EBPtr<EBSqliteStatement> ptr = EBCreate<EBSqliteStatement>(db, query);
        return ptr.cast<EBStatement>();
    }

    bool exec( EBString query )
    {
        EB_LOG_DEBUG("Execute: " << query);

        char * errorMessage = nullptr;
        int rc = sqlite3_exec(db, query.dataPtr(), NULL, NULL, &errorMessage);
        if( rc != SQLITE_OK )
        {
            EB_LOG_ERROR("SQL error: " << errorMessage);
            sqlite3_free(errorMessage);
            return false;
        }
        return true;
    }

private:
    struct sqlite3 * db;

};

}