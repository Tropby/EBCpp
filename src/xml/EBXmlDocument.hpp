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
#include "../EBFile.hpp"

#include "EBXmlParser.hpp"

#include <dirent.h>

namespace EBCpp
{

class EBXmlDocument : public EBObject<EBXmlDocument>
{
public:
    EBXmlDocument() : version("1.0")
    {
        root = nullptr;
    }

    bool loadFromMemory(const EBString& data)
    {
        EBXmlParser parser;
        root = parser.parse(data);

        version = parser.getVersion();
        standalone = parser.getStandalone();
        encoding = parser.getEncoding();

        return (root != nullptr);
    }

    bool loadFromFile(const EBString& filename)
    {
        EBFile file;
        file.setFileName(filename);
        if( file.open(EBFile::READ_ONLY) )
        {
            EBXmlParser parser;
            root = parser.parse(file.readAll(), false);
        }
        else
        {
            root = nullptr;
        }

        return (root != nullptr);
    }

    const EBString dump(bool readable = false)
    {
        EBString result = "<?xml ";

        if( !version.empty() )
        {
            result += "version=\"" + version + "\" ";
        }

        if( !encoding.empty() )
        {
            result += "encoding=\"" + encoding + "\" ";
        }

        if( !standalone.empty() )
        {
            result += "standalone=\"" + standalone + "\" ";
        }
        result += "?>";

        return result + "\n" + this->privateDump(root, readable, 0);
    }

    void setVersion(const EBString& version) 
    {
        this->version = version;
    }

    void setEncoding(const EBString& encoding) 
    {
        this->encoding = encoding;
    }

    void setStandalone(const EBString& standalone) 
    {
        this->standalone = standalone;
    }

    const EBString getVersion() const
    {
        return version;
    }

    const EBString getEncoding() const
    {
        return encoding;
    }

    const EBString getStandalone() const
    {
        return standalone;
    }

    void setRootElement(const EBPtr<EBXmlElement> element)
    {
        root = element;
    }

    const EBPtr<EBXmlElement> getRootElement() const
    {
        return root;
    }

private:
    
    const EBString privateDump(EBPtr<EBXmlElement> element, bool readable, int level)
    {
        EBString result;
        if (readable)
        {
            for (int i = 0; i < level; i++)
            {
                result += "    ";
            }
        }

        switch (element->getType())
        {
        case EBXmlElement::TYPE::ELEMENT: {
            result += "<" + element->getName();

            if (element->getAttributes().getSize())
            {
                if (readable)
                    result += "\n";
                for (const auto a : element->getAttributes())
                {
                    result += privateDump(a.get(), readable, level + 1);
                }
            }

            if (element->getChildren().getSize())
            {
                if (readable && element->getAttributes().getSize())
                {
                    for (int i = 0; i < level; i++)
                    {
                        result += "    ";
                    }
                }
                result += ">";
                if (readable)
                    result += "\n";

                for (const auto a : element->getChildren())
                {
                    result += privateDump(a.get(), readable, level + 1);
                }
                
                if (readable)
                {
                    for (int i = 0; i < level; i++)
                    {
                        result += "    ";
                    }
                }

                result += "</" + element->getName() + ">";
            }
            else
            {
                result += " />";
            }
            if (readable)
                result += "\n";

            break;
        }

        case EBXmlElement::TYPE::ATTRIBUTE: {
            result += " " + element->getName() + "=\"" + element->getValue().replace("\"", "\\\"") + "\"";
            if (readable)
                result += "\n";
            break;
        }

        case EBXmlElement::TYPE::TEXT: 
        {
            result += element->getValue();
            if (readable)
                result += "\n";
            break;
        }
        }

        return result;
    }

    EBPtr<EBXmlElement> root;

    EBString version;
    EBString encoding;
    EBString standalone;    
};

} // namespace EBCpp