#pragma once

#include "EBJsonValue.hpp"
#include "../EBUtils.hpp"

namespace EBCpp {

class EBJsonBoolean : public EBJsonValue
{
public:
    EBJsonBoolean() : value(false)
    {

    }

    EBJsonBoolean(const bool value) : value(value)
    {

    }

    EBJsonBoolean(const EBJsonBoolean& other) : value(other.value)
    {

    }

    EBJsonBoolean& operator=(const EBJsonBoolean &json)
    {
        set(json.get());
        return *this;
    }

    EBJsonBoolean& operator=(const bool &value)
    {
        set(value);
        return *this;
    }

    void set( const bool value )
    {
        this->value = value;
    }

    const bool get() const
    {
        return value;
    }

    virtual const EBString dump(int level = 0) const
    {
        return (value ? "true" : "false");
    }

    virtual const int parse( EBString data )
    {
        if( data.startsWith("true") )
        {
            value = true;
            return 3;
        }
            
        if( data.startsWith("false") )
        {
            value = false;
            return 4;
        }

        EB_EXCEPTION("EBJsonBoolean is not a boolean value!");
    }

private:
    bool value;
};

}