#pragma once

#include "EBJsonValue.hpp"

namespace EBCpp {

class EBJsonString : public EBJsonValue
{
public:
    EBJsonString()
    {

    }

    EBJsonString(const EBString value) : value(value)
    {

    }

    EBJsonString(const EBJsonString& other) : value(other.value)
    {

    }

    EBJsonString& operator=(const EBJsonString &json)
    {
        set(json.get());
        return *this;
    }

    EBJsonString& operator=(const EBString &value)
    {
        set(value);
        return *this;
    }

    void set( const EBString& value )
    {
        this->value = value.replace("\"", "\\\"");
    }

    const EBString& get() const
    {
        return value;
    }

    virtual const EBString dump(int level = 0) const
    {
        return EBString("\"") + value + "\"";
    }

    virtual const int parse( EBString data)
    {
        int i;
        bool escape = false;
        for( i = 0; i < data.length(); i++ )
        {
            if( data.mid(i,1) == "\"" && !escape )
            {
                i++;
                break;
            }
            else if( data.mid(i,1) == "\\" && !escape )
            {
                escape = true;
            }
            else
            {                
                value += data.mid(i,1);
                escape = false;
            }
        }
        return i;
    }

private:
    EBString value;
};

}