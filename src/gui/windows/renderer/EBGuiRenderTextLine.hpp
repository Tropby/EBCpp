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

#include "EBGuiRenderTextLine.hpp"
#include "../../renderer/EBGuiRect.hpp"
#include <codecvt>
#include <locale>

#ifdef __WIN32__

namespace EBCpp
{

class EBGuiRenderTextLine : public EBGuiRenderTextLineBase
{
public:
    EBGuiRenderTextLine(EBObject* parent, int x, int y, int w, std::string text,
                        EBGuiColor fontColor = EB_COLOR_BLACK) :
        EBGuiRenderTextLineBase(parent, x, y, w, text, fontColor)
    {
    }

    virtual void render(Gdiplus::Graphics& graphics)
    {
        Gdiplus::FontFamily fontFamily(L"Times New Roman");
        Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::PointF pointF(x, y);
        Gdiplus::StringFormat stringFormat;
      
        Gdiplus::SolidBrush solidBrush(
        Gdiplus::Color(fontColor.getA(), fontColor.getR(), fontColor.getG(), fontColor.getB()));

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring w = converter.from_bytes(text);

        graphics.DrawString(w.c_str(), -1, &font, pointF, &stringFormat, &solidBrush);
    }

protected:
};

} // namespace EBCpp

#endif