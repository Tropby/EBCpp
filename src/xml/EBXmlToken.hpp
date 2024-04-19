/*
 * EBCpp Project
 * https://github.com/Tropby/EBCpp
 *
 * ---
 *
 * MIT License
 *
 * Copyright (c) 2024 Carsten (Tropby)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "../EBList.hpp"
#include "../EBObject.hpp"
#include "../EBString.hpp"

namespace EBCpp
{

class EBXmlToken : public EBObject<EBXmlToken>
{
public:
    enum class TokenId{
        TEXT,
        TAG_BEGIN,
        TAG_END,
        ATTRIBUTE_NAME,
        ATTRIBUTE_VALUE,
        UNDEFINED = -1
    };

    EBXmlToken(TokenId id, EBString value) : id(id), value(value)
    {
    }

    EBXmlToken(const EBXmlToken& other)
    {
        this->id = other.id;
        this->value = other.value;
    }

    EBXmlToken& operator=(const EBXmlToken& other)
    {
        // Guard self assignment
        if (this == &other)
            return *this;

        this->id = other.id;
        this->value = other.value;

        return *this;
    }

    EBString getIdName() const
    {
        switch( id )
        {
            case TokenId::ATTRIBUTE_NAME:
                return "TokenId::ATTRIBUTE_NAME";
            case TokenId::ATTRIBUTE_VALUE:
                return "TokenId::ATTRIBUTE_VALUE";
            case TokenId::TAG_BEGIN:
                return "TokenId::TAG_BEGIN";
            case TokenId::TAG_END:
                return "TokenId::TAG_END";
            case TokenId::TEXT:
                return "TokenId::TEXT";
            default:
                return "TokenId::UNDEFINED";
        }
    }

    TokenId getId() const
    {
        return id;
    }

    EBString getValue() const
    {
        return value;
    }

private:
    TokenId id;
    EBString value;
};

} // namespace EBCpp