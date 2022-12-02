#pragma once

#include "EBJsonValue.hpp"
#include "EBJsonString.hpp"
#include "EBJsonNumber.hpp"
#include "EBJsonNull.hpp"
#include "EBJsonBoolean.hpp"
#include "../EBMap.hpp"

namespace EBCpp {

template<bool isArray = false>
class EBJsonObjectBase : public EBJsonValue
{
public:
    EBJsonObjectBase()
    {

    }

    EBJsonObjectBase(const EBJsonObjectBase& other) : values(other.values)
    {

    }

    virtual void append(const EBJsonObjectBase<>& value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const EBJsonString& value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const EBString& value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const double value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const int value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const EBJsonNumber& value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const EBJsonNull& value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void append(const EBJsonBoolean& value)
    {
        if( !isArray )
            EB_EXCEPTION("Can not append to object!");

        set( EBUtils::intToStr(values.size()), value );
    }

    virtual void set(const EBString& key, const EBJsonObjectBase<false>& value)
    {        
        values[key] = EBCreate<EBJsonObjectBase<false>>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const EBJsonObjectBase<true>& value)
    {        
        values[key] = EBCreate<EBJsonObjectBase<true>>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const EBJsonString& value)
    {        
        values[key] = EBCreate<EBJsonString>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const EBString& value)
    {
        values[key] = EBCreate<EBJsonString>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const EBJsonNumber& value)
    {
        values[key] = EBCreate<EBJsonNumber>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const EBJsonBoolean& value)
    {
        values[key] = EBCreate<EBJsonBoolean>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const EBJsonNull& value)
    {
        values[key] = EBCreate<EBJsonNull>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const double& value)
    {
        values[key] = EBCreate<EBJsonNumber>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const int& value)
    {
        values[key] = EBCreate<EBJsonNumber>(value).cast<EBJsonValue>();
    }

    virtual void set(const EBString& key, const bool& value)
    {
        values[key] = EBCreate<EBJsonBoolean>(value).cast<EBJsonValue>();
    }

    EBPtr<EBJsonValue> get(EBString key)
    {
        return values[key];
    }

    const int size() const
    {
        return values.size();
    }

    EBPtr<EBJsonValue> at(int index)
    {
        return values[EBUtils::intToStr(index)];
    }

    virtual const EBString dump(int level = -1) const
    {
        if( isArray )
        {
            EBString result = (level>=0?EBString("\n"):EBString("")) + doLevel(level) + "[" + (level>=0?EBString("\n"):EBString(""));
            bool first = true;
            for( auto value : values)
            {
                if( !first )
                    result += EBString(", ") + (level>=0?EBString("\n"):EBString(""));
                result += doLevel(level+1) + value.second.get()->dump(level>=0?level+1:level);
                first = false;
            }
            return result + (level>=0?EBString("\n"):EBString("")) + doLevel(level) + "]";
        }

        EBString result = (level>=0?EBString("\n"):EBString("")) + doLevel(level) + EBString("{") + (level>=0?EBString("\n"):EBString(""));
        bool first = true;
        for( auto value : values )
        {
            if( !first )
                result += EBString(", ")+ (level>=0?EBString("\n"):EBString(""));
            result += doLevel(level+1) + EBString("\"") + value.first + "\": " + value.second->dump(level>=0?level+1:level);
            first = false;
        }
        return result + (level>=0?EBString("\n"):EBString("")) + doLevel(level) + "}" ;
    }

    virtual const int parseArray(EBString data, int level = 0)
    {

        enum {
            VALUE_START,
            VALUE_END            
        }state = VALUE_START;

        bool escape = false;
        int key = 0;
        int i;
        for( i = 0; i < data.length(); i++)
        {           
            switch(state)
            {
                case VALUE_START:

                    // STRING FOUND
                    if( data.mid(i,1) == "\"")
                    {
                        EBPtr<EBJsonString> s = EBCreate<EBJsonString>();
                        i += s->parse(data.mid(i+1));
                        values[EBUtils::intToStr(key++)] = s.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // NUMBER FOUND
                    else if( data.mid(i,1).dataPtr()[0] >= '0' && data.mid(i,1).dataPtr()[0] <= '9' )
                    {
                        EBPtr<EBJsonNumber> s = EBCreate<EBJsonNumber>();
                        i += s->parse(data.mid(i));
                        values[EBUtils::intToStr(key++)] = s.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // OBJECT FOUND
                    else if( data.mid(i,1) == "{" )
                    {
                        EBPtr<EBJsonObjectBase<false>> o = EBCreate<EBJsonObjectBase<false>>();
                        i += o->parse(data.mid(i));
                        values[EBUtils::intToStr(key++)] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // ARRAY FOUND
                    else if( data.mid(i,1) == "[" )
                    {
                        EBPtr<EBJsonObjectBase<true>> o = EBCreate<EBJsonObjectBase<true>>();
                        i += o->parse(data.mid(i));
                        values[EBUtils::intToStr(key++)] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // TRUE FOUND
                    else if( data.mid(i,1) == "t" || data.mid(i,1) == "f" )
                    {
                        EBPtr<EBJsonBoolean> o = EBCreate<EBJsonBoolean>();
                        i += o->parse(data.mid(i));
                        values[EBUtils::intToStr(key++)] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // NULL FOUND
                    else if( data.mid(i,1) == "n" )
                    {
                        EBPtr<EBJsonNull> o = EBCreate<EBJsonNull>();
                        i += o->parse(data.mid(i));
                        values[EBUtils::intToStr(key++)] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    break;

                case VALUE_END:
                    if( data.mid(i,1) == "]")
                    {
                        i++;
                        return i;
                    }
                    else if( data.mid(i,1) == ",")
                    {
                        state = VALUE_START;
                    }
                    break;                    
            }
        }

        return i;
    }

    virtual const int parseObject(EBString data)
    {
        enum {
            KEY_START,
            KEY_END,
            SEPERATOR,
            VALUE_START,
            VALUE_END            
        }state = KEY_START;

        bool escape = false;
        EBString key;
        int i;
        for( i = 0; i < data.length(); i++)
        {           
            switch(state)
            {
                case KEY_START:
                    if( data.mid(i,1) == "}")
                    {
                        i++;
                        return i;
                    }
                    else if( data.mid(i,1) == "\"" )
                    {
                        key = "";
                        state = KEY_END;
                    }
                    break;

                case KEY_END:
                    if( data.mid(i,1) == "\"" && !escape )
                    {
                        state = SEPERATOR;
                    }
                    else if( data.mid(i,1) == "\\" && !escape)
                    {
                        escape = true;
                    }
                    else
                    {
                        key += data.mid(i,1);
                        escape = false;
                    }
                    break;

                case SEPERATOR:
                    if( data.mid(i,1) == ":" )
                    {
                        state = VALUE_START;
                    }
                    break;

                case VALUE_START:

                    // STRING FOUND
                    if( data.mid(i,1) == "\"")
                    {
                        EBPtr<EBJsonString> s = EBCreate<EBJsonString>();
                        i += s->parse(data.mid(i+1));
                        values[key] = s.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // NUMBER FOUND
                    else if( data.mid(i,1).dataPtr()[0] >= '0' && data.mid(i,1).dataPtr()[0] <= '9' )
                    {
                        EBPtr<EBJsonNumber> s = EBCreate<EBJsonNumber>();
                        i += s->parse(data.mid(i));
                        values[key] = s.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // OBJECT FOUND
                    else if( data.mid(i,1) == "{" )
                    {
                        EBPtr<EBJsonObjectBase<false>> o = EBCreate<EBJsonObjectBase<false>>();
                        i += o->parse(data.mid(i));
                        values[key] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // ARRAY FOUND
                    else if( data.mid(i,1) == "[" )
                    {
                        EBPtr<EBJsonObjectBase<true> > o = EBCreate<EBJsonObjectBase<true> >();
                        i += o->parse(data.mid(i));
                        values[key] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }


                    // TRUE/FALSE FOUND
                    else if( data.mid(i,1) == "t" || data.mid(i,1) == "f" )
                    {
                        EBPtr<EBJsonBoolean> o = EBCreate<EBJsonBoolean>();
                        i += o->parse(data.mid(i));
                        values[key] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }

                    // NULL FOUND
                    else if( data.mid(i,1) == "n" )
                    {
                        EBPtr<EBJsonNull> o = EBCreate<EBJsonNull>();
                        i += o->parse(data.mid(i));
                        values[key] = o.cast<EBJsonValue>();
                        state = VALUE_END;
                    }


                    break;

                case VALUE_END:
                    if( data.mid(i,1) == "}")
                    {
                        i++;
                        return i;
                    }
                    else if( data.mid(i,1) == ",")
                    {
                        state = KEY_START;
                    }
                    break;                    
            }
        }

        return i;
    }

    virtual const int parse( EBString data)
    {
        // Trim white spaces
        data = data.trim();

        if( !( data.startsWith("{") && !isArray ) && !( data.startsWith("[") && isArray ) )
            EB_EXCEPTION("Can not interpret EBJsonArray!");

        data = data.mid(1);

        if( isArray )
        {
            return parseArray(data);
        }
        return parseObject(data);
    }

private:
    EBMap<EBString, EBPtr<EBJsonValue> > values;
};

typedef EBJsonObjectBase<true> EBJsonArray;
typedef EBJsonObjectBase<false> EBJsonObject;

}
