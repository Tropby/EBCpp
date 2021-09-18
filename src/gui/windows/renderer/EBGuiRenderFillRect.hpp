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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBGuiRenderer.hpp"

#ifdef __WIN32__

namespace EBCpp
{

class EBGuiRenderFillRect : public EBGuiRenderFillRectBase
{
public:
    EBGuiRenderFillRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor> backgroundColor = EB_COLOR_WHITE) :
        EBGuiRenderFillRectBase(x, y, w, h, backgroundColor)
    {
    }

    virtual void render(Gdiplus::Graphics& graphics)
    {
        Gdiplus::SolidBrush brush(Gdiplus::Color(backgroundColor->getA(), backgroundColor->getR(),
                                                    backgroundColor->getG(), backgroundColor->getB()));
        graphics.FillRectangle(&brush, x, y, w, h);
    }

protected:
};

} // namespace EBCpp

#endif