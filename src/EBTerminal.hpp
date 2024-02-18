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
 *  Created on: 2022-06-19
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBEventLoop.hpp"
#include "EBList.hpp"
#include "EBObject.hpp"
#include "EBString.hpp"

#ifdef WIN32

namespace EBCpp
{

class EBTerminal : public EBObject<EBTerminal>
{
public:
    /**
     * @brief Colors defined for the terminal
     * 
     */
    typedef enum
    {
        black   = 0x0,
        navy    = 0x1,
        green   = 0x2,
        teal    = 0x3,
        maroon  = 0x4,
        purple  = 0x5,
        olive   = 0x6,
        silver  = 0x7,
        gray    = 0x8,
        blue    = 0x9,
        lime    = 0xA,
        aqua    = 0xB,
        red     = 0xC,
        fuchsia = 0xD,
        yellow  = 0xE,
        white   = 0xF
    } TERMINAL_COLOR;

    /**
     * @brief Set the Color of the terminal
     * 
     * @param backgroundColor The background color of the terminal
     * @param textColor The foreground color of the terminal
     */
    static void setColor(TERMINAL_COLOR backgroundColor, TERMINAL_COLOR textColor)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        WORD saved_attributes;

        /* Save current attributes */
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
        saved_attributes = consoleInfo.wAttributes;

        SetConsoleTextAttribute(hConsole, ( textColor & 0x0F ) | ( backgroundColor & 0x0F ) << 4 );
    }

private:
};

} // namespace EBCpp

#endif