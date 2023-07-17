#pragma once

#include "EBJsonValue.hpp"
#include "../EBUtils.hpp"

namespace EBCpp {

class EBJsonNumber : public EBJsonValue
{
public:
    EBJsonNumber() : value(0.0)
    {

    }

    EBJsonNumber(const double value) : value(value)
    {

    }

    EBJsonNumber(const EBJsonNumber& other) : value(other.value)
    {

    }

    EBJsonNumber& operator=(const EBJsonNumber &json)
    {
        set(json.get());
        return *this;
    }

    EBJsonNumber& operator=(const double &value)
    {
        set(value);
        return *this;
    }

    void set( const double value )
    {
        this->value = value;
    }

    const double get() const
    {
        return value;
    }

    virtual const EBString dump(int level = 0) const
    {
        return EBUtils::doubleToStr(value);
    }

    virtual const int parse( EBString data )
    {
        int i;
        EBString v;
        for( i = 0; i < data.length(); i++ )
        {
            char c = data.dataPtr()[i];
            if( (c >= '0' && c <= '9') || c == '.')
            {
                v += c;
            }
            else
            {                
                break;
            }
        }
        value = v.toDouble();
        return i-1;
    }

private:
    double value;
};

}