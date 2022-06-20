/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten (Tropby)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *  Created on: 2022-06-19
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBEventLoop.hpp"
#include "EBList.hpp"
#include "EBObject.hpp"
#include "EBString.hpp"

namespace EBCpp
{

class EBTerminal : public EBObject<EBTerminal>
{
public:
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
