/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#define EB_COLOR_BLACK EBGuiColor(nullptr, 0x00, 0x00, 0x00, 0xFF)
#define EB_COLOR_RED EBGuiColor(nullptr, 0xFF, 0x00, 0x00, 0xFF)
#define EB_COLOR_BLUE EBGuiColor(nullptr, 0x00, 0xFF, 0x00, 0xFF)
#define EB_COLOR_GREEN EBGuiColor(nullptr, 0x00, 0x00, 0xFF, 0xFF)

namespace EBCpp
{

class EBGuiColor : public EBObject
{
public:
    EBGuiColor(EBObject* parent, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF) :
        EBObject(parent), r(r), g(g), b(b), a(a)
    {
    }

    EBGuiColor(const EBGuiColor & other) : 
        EBObject(other.getParent()), r(other.r), g(other.g), b(other.b), a(other.a)
    {        
    }

    unsigned char getR()
    {
        return r;
    }

    unsigned char getG()
    {
        return g;
    }

    unsigned char getB()
    {
        return b;
    }

    unsigned char getA()
    {
        return a;
    }

protected:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

} // namespace EBCpp
