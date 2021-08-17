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

#include "EBGuiRenderer.hpp"
#include "EBGuiColor.hpp"

namespace EBCpp
{

class EBGuiRenderTextBase : public EBGuiRenderer
{
public:
    EBGuiRenderTextBase(EBObject* parent, int x, int y, int w, int h, std::string text,
                        EBGuiColor fontColor = EB_COLOR_BLACK) :
        EBGuiRenderer(parent),
        x(x), y(y), w(w), h(h), text(text), fontColor(fontColor)
    {
    }

protected:
    int x;
    int y;
    int w;
    int h;
    std::string text;
    EBGuiColor fontColor;
};

} // namespace EBCpp

#ifdef __WIN32__
#include "../windows/renderer/EBGuiRenderText.hpp"
#endif