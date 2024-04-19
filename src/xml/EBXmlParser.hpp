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

#include "EBXmlToken.hpp"
#include "EBXmlElement.hpp"
#include "EBXmlException.hpp"

namespace EBCpp
{

class EBXmlParser : public EBObject<EBXmlParser>
{
public:
    EBXmlParser(){}

    EBPtr<EBXmlElement> parse(const EBString& data, bool checkDeclaration = true)
    {
        tokenize(data, checkDeclaration);
        return createNodesFromTokenList();
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

private:

    int getNextDelimiter(EBString& data, int startPos)
    {
        int delimiterSpace = data.indexOf(" ", startPos);
        int delimiterClosing = data.indexOf(">", startPos);
        int delimiterEquals = data.indexOf("=", startPos);
        int delimiterTagBegin = data.indexOf("<", startPos);

        int end = delimiterClosing;
        if( end > delimiterEquals && delimiterEquals != -1)
            end = delimiterEquals;
        if( end > delimiterSpace && delimiterSpace != -1)
            end = delimiterSpace;
        if( end > delimiterTagBegin && delimiterTagBegin != -1)
            end = delimiterTagBegin;

        return end;
    }

    int getDeclaration(const EBString& data)
    {
        if( !data.trim().startsWith( "<?xml" ) )
            EB_EXCEPTION_EX(EBXmlExceptionDeclaration, EBString("Missing XML-Deklaration!"));

        EBString declaration = data.mid(0, data.indexOf("?>")+2);
        declaration = declaration.replace("<?xml", "<?");
        EBXmlParser parser;
        EBPtr<EBXmlElement> root = parser.parse(declaration, false);

        EBPtr<EBXmlElement> version = root->getAttributeByName("version");
        if( version != nullptr )
        {
            this->version = version->getValue();
        }

        EBPtr<EBXmlElement> standalone = root->getAttributeByName("standalone");
        if( standalone != nullptr )
        {
            this->standalone = standalone->getValue();
        }

        EBPtr<EBXmlElement> encoding = root->getAttributeByName("encoding");
        if( encoding != nullptr )
        {
            this->encoding = encoding->getValue();
        }

        return data.indexOf("?>") + 2;
    }

    void tokenize(const EBString& data, bool checkDeclaration = true)
    {
        int tokenStart = 0;
        int tokenEnd = 0;

        if(checkDeclaration)
        {
            tokenStart = tokenEnd = getDeclaration(data);
        }

        EBXmlToken lastToken(EBXmlToken::TokenId::UNDEFINED, "");

        enum class STATE{
            START,
            TEXT,
            TAG_BEGIN,
            TAG_END,
            ATTRIBUTE_NAME,
            ATTRIBUTE_VALUE
        };
        STATE state = STATE::START;

        char attributeStart = 0x00;
        EBString currentTagName = "";

        while( tokenStart < data.length() )
        {
            char c = data[tokenEnd];

            switch( state )
            {
                case STATE::START:
                {
                    if( c == '<' )
                    {
                        state = STATE::TAG_BEGIN;
                        tokenStart=tokenEnd+1;
                    }
                    break;
                }

                case STATE::TAG_BEGIN:
                {
                    if( c == '/' ) 
                    {
                        if( tokenStart == tokenEnd )
                        {
                            state = STATE::TAG_END;
                            tokenStart++;
                        }
                        else
                        {
                            if( data[tokenEnd+1] == '>' )
                            {
                                state = STATE::TEXT;
                            }
                        }
                    }
                    if( c == '>' || c == ' ' )
                    {
                        EBString t = data.mid(tokenStart, tokenEnd - tokenStart);
                        tokenList.append(EBCpp::EBXmlToken(EBXmlToken::TokenId::TAG_BEGIN, t));
                        currentTagName = t;
                        tokenStart = tokenEnd + 1;
                        if( c == '>' )
                        {
                            state = STATE::TEXT;
                        }
                        else
                        {
                            state = STATE::ATTRIBUTE_NAME;
                        }
                    }
                    break;
                }

                case STATE::TAG_END:
                {
                    if( c == '>' || c == ' ' )
                    {
                        EBString t = data.mid(tokenStart, tokenEnd - tokenStart);
                        tokenList.append(EBCpp::EBXmlToken(EBXmlToken::TokenId::TAG_END, t));
                        tokenStart = tokenEnd + 1;
                        state = STATE::TEXT;
                    }
                    break;
                }                

                case STATE::TEXT:
                {
                    if( c == '<' )
                    {
                        EBString t = data.mid(tokenStart, tokenEnd - tokenStart);
                        if( !t.trim().empty() )
                        {
                            tokenList.append(EBCpp::EBXmlToken(EBXmlToken::TokenId::TEXT, t));
                        }
                        tokenStart = tokenEnd + 1;
                        state = STATE::TAG_BEGIN;
                    }
                    break;
                }

                case STATE::ATTRIBUTE_NAME:
                {
                    if( c == ' ' )
                    {
                        tokenStart = tokenEnd+1;
                    }

                    if( tokenStart == tokenEnd && c == '>')
                    {
                        tokenStart++;
                        state = STATE::TEXT;
                    }

                    if( c == '/' && data[tokenEnd+1] == '>' )
                    {
                        tokenStart = tokenEnd + 2;
                        tokenEnd++;
                        tokenList.append(EBCpp::EBXmlToken(EBXmlToken::TokenId::TAG_END, currentTagName));
                        state = STATE::TEXT;
                    }

                    if( c == '=' )
                    {
                        // TODO: What if the attribute name is empty???
                        EBString t = data.mid(tokenStart, tokenEnd - tokenStart);
                        tokenList.append(EBCpp::EBXmlToken(EBXmlToken::TokenId::ATTRIBUTE_NAME, t));
                        tokenStart = tokenEnd + 2;
                        state = STATE::ATTRIBUTE_VALUE;
                    }
                    break;
                }

                case STATE::ATTRIBUTE_VALUE:
                {
                    if( !attributeStart )
                    {
                        if( c == '"' || c == '\'' )
                        {
                            attributeStart = c;
                        }
                    }
                    else
                    {
                        if( c == attributeStart )
                        {
                            attributeStart = 0x00;
                            EBString t = data.mid(tokenStart, tokenEnd - tokenStart);
                            tokenList.append(EBCpp::EBXmlToken(EBXmlToken::TokenId::ATTRIBUTE_VALUE, t));
                            tokenStart = tokenEnd + 1;
                            state = STATE::ATTRIBUTE_NAME;
                        }
                    }
                    break;
                }
            }

            tokenEnd++;
        }
    }

    EBPtr<EBXmlElement> createNodesFromTokenList()
    {
        EBPtr<EBXmlElement> rootElement = nullptr;
        EBPtr<EBXmlElement> currentElement = nullptr;
        EBPtr<EBXmlElement> currentAttribute = nullptr;

        for( const auto& token : tokenList )
        {
            switch(token.get().getId())
            {
                case EBXmlToken::TokenId::TAG_BEGIN:
                {
                    EBPtr<EBXmlElement> element = EBCreate<EBXmlElement>(EBXmlElement::TYPE::ELEMENT, token.get().getValue());
                    currentAttribute = nullptr;

                    if( currentElement != nullptr )
                    {
                        currentElement->addChild(element);
                    }
                    else
                    {
                        if( rootElement != nullptr )
                        {
                            EB_EXCEPTION_EX(EBXmlExceptionSecondRootElement,
                                         "Second root element: " + token.get().getValue());
                        }
                        rootElement = element;
                    }

                    currentElement = element;                    
                    break;
                }

                case EBXmlToken::TokenId::ATTRIBUTE_NAME:
                {
                    currentAttribute = EBCreate<EBXmlElement>(EBXmlElement::TYPE::ATTRIBUTE, token.get().getValue());
                    currentElement->addAttribute(currentAttribute);
                    break;
                }

                case EBXmlToken::TokenId::ATTRIBUTE_VALUE:
                {
                    currentAttribute->setValue(token.get().getValue());
                    currentAttribute = nullptr;
                    break;
                }

                case EBXmlToken::TokenId::TAG_END:
                {
                    currentAttribute = nullptr;

                    EBString nn = currentElement->getName();
                    EBString cn = token.get().getValue();
                    if( cn != nn )
                    {
                        EB_EXCEPTION_EX(EBXmlExceptionTagEndMissmatch, cn + " != " + nn);
                    }
                    currentElement = currentElement->getParent();                    
                    break;
                }

                case EBXmlToken::TokenId::TEXT:
                {
                    EBPtr<EBXmlElement> element = EBCreate<EBXmlElement>(EBXmlElement::TYPE::TEXT);
                    element->setValue(token.get().getValue());
                    currentElement->addChild(element);
                }
            }
        }
        return rootElement;
    }

    EBList<EBXmlToken> tokenList;

    EBString version;
    EBString encoding;
    EBString standalone;
};

} // namespace EBCpp