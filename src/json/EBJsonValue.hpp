#pragma once

#include "../EBObject.hpp"
#include "../EBString.hpp"

namespace EBCpp {

class EBJsonValue : public EBObject<EBJsonValue>
{
public:
    virtual const EBString dump(int level = 0) const
    {
        return "";
    }

protected:
    const EBString doLevel(int level) const
    {
        EBString result;
        for( int i = 0; i < level; i++ )
        {
            result += "\t";
        }
        return result;
    }
};

}