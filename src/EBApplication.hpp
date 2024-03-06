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
 *  Created on: 2022-06-17
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBObject.hpp"
#include "EBList.hpp"
#include "EBString.hpp"
#include "EBEventLoop.hpp"

#define EB_APPLICATION(MAIN_CLASS)                                                                                     \
    EBPtr<MAIN_CLASS> mainClass = nullptr;                                                                             \
    bool EBInit()                                                                                                      \
    {                                                                                                                  \
        mainClass = EBCreate<MAIN_CLASS>();                                                                            \
        return true;                                                                                                   \
    }                                                                                                                  \
    void EBShutdown()                                                                                                  \
    {                                                                                                                  \
    }                                                                                                                  \
    int main(int argc, char** argv)                                                                                    \
    {                                                                                                                  \
        EBCpp::EBApplication::setArguments(argc, argv);                                                                \
        if (EBInit())                                                                                                  \
        {                                                                                                              \
            EBCpp::EBEventLoop::getInstance()->exec();                                                                 \
            EBShutdown();                                                                                              \
        }                                                                                                              \
        return 0;                                                                                                      \
    }

namespace EBCpp
{

class EBApplication : public EBObject<EBApplication>
{
public:
    static void setArguments( int argc, char ** argv )
    {
        for( int i = 0; i < argc; i++ )
        {
            arguments.append(argv[i]);
        }
    }

    static bool containsArgument( const EBString& argument )
    {
        return arguments.contains(argument);
    }

    static EBString getArgument( int index )
    {
        return arguments.get(index);
    }

    static int getArgumentCount()
    {
        return arguments.getSize();
    }

private:
    EBApplication(){}
    static inline EBList<EBString> arguments = EBList<EBString>();

};


} // namespace EBCpp

bool EBInit();
void EBShutdown();

