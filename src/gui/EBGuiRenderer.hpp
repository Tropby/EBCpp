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

#include "../EBString.hpp"
#include "EBGuiAlignment.hpp"
#include "EBGuiColor.hpp"

namespace EBCpp
{

class EBGuiRendererBase : public EBObject<EBGuiRendererBase>
{
public:
    EBGuiRendererBase(int x, int y, int width, int height) : EBObject(), x(x), y(y), width(width), height(height)
    {
    }

    int getWidth()
    {
        return width;
    }

    int getHeight()
    {
        return height;
    }

    int getX()
    {
        return x;
    }

    int getY()
    {
        return y;
    }

    virtual void drawRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor>& borderColor) = 0;
    virtual void drawFillRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor> backgroundColor) = 0;

    virtual void drawText(int x, int y, int w, int h, EBString text, const EBObjectPointer<EBGuiColor>& fontColor,
                          EBGuiHorizontalAlignment horizontalAlignment = EBGuiHorizontalAlignment::EB_HOR_ALIGN_LEFT,
                          EBGuiVerticalAlignment verticalAlignment = EBGuiVerticalAlignment::EB_VERT_ALIGN_TOP) = 0;

    virtual void drawTextWithCursor(int x, int y, int w, int h, EBString text, int cursorPosition,
                                    const EBObjectPointer<EBGuiColor>& fontColor) = 0;

private:
    int x;
    int y;
    int width;
    int height;
};

} // namespace EBCpp

#ifdef __WIN32__
#include "windows/EBGuiRenderer.hpp"
#else
#warning "Can not use EBGuiRenderer on this platform!"
#endif
