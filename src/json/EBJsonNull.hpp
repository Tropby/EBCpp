#pragma once

#include "EBJsonValue.hpp"
#include "../EBUtils.hpp"

namespace EBCpp {

class EBJsonNull : public EBJsonValue
{
public:
    EBJsonNull()
    {

    }

    EBJsonNull(const EBJsonNull& other)
    {

    }

    EBJsonNull& operator=(const EBJsonNull &json)
    {
        return *this;
    }

    virtual const EBString dump(int level = 0) const
    {
        return "null";
    }

    virtual const int parse( EBString data )
    {
        if( data.startsWith("null") )
            return 3;

        EB_EXCEPTION("EBJsonNull: Null is not null!");
    }
};

}