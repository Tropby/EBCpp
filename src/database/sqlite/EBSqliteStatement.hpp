#pragma once

#include "../EBStatement.hpp"

#include <sqlite3.h>

namespace EBCpp {

class EBSqliteStatement : public EBStatement
{
public:
    EBSqliteStatement(sqlite3* db, EBString query) : EBStatement(query), db(db)
    {
        EB_LOG_INFO("Prepare EBSqliteStatement: " << query);
        if (sqlite3_prepare_v2(db, query.dataPtr(), -1, &stmt, NULL)) 
        {
            EB_LOG_ERROR("Error preparing sql statement\n");
        }
    }

    virtual ~EBSqliteStatement()
    {
        EB_LOG_INFO("Finish EBSqliteStatement: " << query);
        sqlite3_finalize(stmt);
    }

    /*
    * @brief Binds a parameter. The string must exist as long as the statement lives.
    */
    void bind(const int index, const EBString& value)
    {
        sqlite3_bind_text (stmt, index, value.dataPtr(), -1, NULL);
    }

    void bind(const int index, const int value)
    {
        sqlite3_bind_int (stmt, index, value);        
    }

    bool exec()
    {
        return sqlite3_step(stmt) == SQLITE_DONE;
    }

    bool next()
    {
        return (sqlite3_step(stmt) == SQLITE_ROW);
    }

    int getInt(int column)
    {
        return sqlite3_column_int(stmt, column);
    }

    EBString getString(int column)
    {
        const char * c = (const char*)sqlite3_column_text(stmt, column);
        if( c != nullptr )
            return c;
        return EBString();
    }

private:
    sqlite3_stmt *stmt;
    sqlite3* db;
};

}