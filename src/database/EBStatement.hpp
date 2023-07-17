#pragma once

#include "../EBObject.hpp"
#include "../EBString.hpp"


namespace EBCpp {

class EBStatement : public EBObject<EBStatement>
{
public:
    EBStatement(EBString query) : query(query)
    {
    }

    virtual void bind(const int index, const EBString& value) = 0;
    virtual void bind(const int index, const int value) = 0;
    virtual bool exec() = 0;
    virtual bool next() = 0;

    virtual int getInt(int column) = 0;
    virtual EBString getString(int column) = 0;

protected:
    EBString query;
};

}