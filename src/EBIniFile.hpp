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

#include "EBFile.hpp"

namespace EBCpp {

class EBIniFile : public EBObject<EBIniFile>
{
public:
    EBIniFile(const EBString& fileName) : fileName(fileName)
    {
        EBString currentSection = "";

        EBFile file;
        file.setFileName(fileName);
        if( file.open(EBFile::READ_ONLY) )
        {
            while( file.canReadLine() )
            {
                EBString line = file.readLine();
                line = line.trim();

                if (line.startsWith(";"))
                {
                    // Kommentarzeile
                    continue;
                }

                if (line.startsWith("["))
                {
                    currentSection = line.mid(1, line.length() - 2);
                }
                else
                {
                    if(line.indexOf("=") != -1 )
                    {
                        EBString key = line.mid(0, line.indexOf("="));
                        EBString value = line.mid(line.indexOf("=")+1);

                        SKV skv;

                        skv.section = currentSection;
                        skv.key = key;
                        skv.value = value;

                        values.append(skv);
                    }
                }
            }
        }
    }

    auto getSection(const EBString& section)
    {
        EBMap<EBString, EBString> result;

        for( auto v : values )
        {
            if( v.get().section == section )
            {
                result.insert(std::pair<EBString, EBString>(v.get().key, v.get().value));
            }
        }
        return result;
    }

    const EBString getValue(const EBString& section, const EBString& key) const
    {
        for( auto v : values )
        {
            if( v.get().section == section && v.get().key == key)
            {
                return v.get().value;
            }
        }
        return "";
    }

private:
    EBString fileName;

    typedef struct {
        EBString section;
        EBString key;
        EBString value;
    } SKV;

    EBList<SKV> values;
};

}