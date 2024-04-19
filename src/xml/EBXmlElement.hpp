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

class EBXmlElement : public EBObject<EBXmlElement>
{
public:
    enum class TYPE {
        ELEMENT,
        TEXT,
        ATTRIBUTE
    };

    EBXmlElement(const TYPE type, const EBString& name = "", const EBString& value = "") : type(type), name(name), value(value){}

    const EBPtr<EBXmlElement> addChild(EBPtr<EBXmlElement> childElement)
    {
        children.append(childElement);

        childElement->setParent(this);

        return this;
    }

    const EBList<EBPtr<EBXmlElement>>& getChildren() const
    {
        return children;
    }

    const EBPtr<EBXmlElement> addAttribute(EBPtr<EBXmlElement> attribute)
    {
        attributes.append(attribute);
        attribute->setParent(this);
        return this;
    }

    const EBList<EBPtr<EBXmlElement>>& getAttributes() const
    {
        return attributes;
    }

    const EBPtr<EBXmlElement> getAttributeByName(const EBString& name) const
    {
        for( const auto& att : attributes )
        {
            if( att.get()->getName() == name )
            {
                return att.get().get();
            }
        }
        return nullptr;
    }

    const EBList<EBPtr<EBXmlElement>> getAttributesByName(const EBString& name) const
    {
        EBList<EBPtr<EBXmlElement>> result;

        for( const auto& att : attributes )
        {
            if( att.get()->getName() == name )
            {
                result.append(att.get().get());
            }
        }
        return result;
    }

    const EBPtr<EBXmlElement> getChildByName(const EBString& name) const
    {
        for( const auto& ch : children )
        {
            if( ch.get()->getName() == name )
            {
                return ch.get().get();
            }
        }
        return nullptr;
    }

    const EBList<EBPtr<EBXmlElement>> getChildrenByName(const EBString& name) const
    {
        EBList<EBPtr<EBXmlElement>> result;
        for( const auto& ch : children )
        {
            if( ch.get()->getName() == name )
            {
                result.append(ch.get().get());
            }
        }
        return result;
    }

    const EBPtr<EBXmlElement> setParent(EBPtr<EBXmlElement> parentElement)
    {
        parent = parentElement;

        return this;
    }

    const EBPtr<EBXmlElement> getParent() const
    {
        return parent;
    }

    const EBString getName() const
    {
        return name;
    }

    const EBString getValue() const
    {
        return value;
    }

    void setValue(const EBString& value)
    {
        this->value = value;
    }

    TYPE getType()
    {
        return type;
    }

private:
    EBString name;
    EBString value;
    EBPtr<EBXmlElement> parent;
    EBList<EBPtr<EBXmlElement>> children;
    EBList<EBPtr<EBXmlElement>> attributes;

    TYPE type;
};

} // namespace EBCpp