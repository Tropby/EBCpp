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
#include <codecvt>
#include <locale>

#ifdef __WIN32__

namespace EBCpp
{

class EBGuiRenderText : public EBGuiRenderTextBase
{
public:
    EBGuiRenderText(int x, int y, int w, int h, std::string text,
                    EBObjectPointer<EBGuiColor> fontColor = EB_COLOR_BLACK,
                    EBGuiHorizontalAlignment horizontalAlignment = EBGuiHorizontalAlignment::EB_HOR_ALIGN_LEFT,
                    EBGuiVerticalAlignment verticalAlignment = EBGuiVerticalAlignment::EB_VERT_ALIGN_TOP) :
        EBGuiRenderTextBase(x, y, w, h, text, fontColor, horizontalAlignment, verticalAlignment)
    {
    }

    virtual void render(Gdiplus::Graphics& graphics)
    {
        Gdiplus::FontFamily fontFamily(L"Times New Roman");
        Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::RectF rectF(x, y, w, h);
        Gdiplus::StringFormat stringFormat;

        switch (horizontalAlignment)
        {
        case EBGuiHorizontalAlignment::EB_HOR_ALIGN_LEFT:
            stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
            break;
        case EBGuiHorizontalAlignment::EB_HOR_ALIGN_RIGHT:
            stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
            break;
        case EBGuiHorizontalAlignment::EB_HOR_ALIGN_CENTER:
            stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
            break;
        }

        switch (verticalAlignment)
        {
        case EBGuiVerticalAlignment::EB_VERT_ALIGN_TOP:
            stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
            break;
        case EBGuiVerticalAlignment::EB_VERT_ALIGN_CENTER:
            stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
            break;
        case EBGuiVerticalAlignment::EB_VERT_ALIGN_BOTTOM:
            stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
            break;
        }

        Gdiplus::SolidBrush solidBrush(
        Gdiplus::Color(fontColor->getA(), fontColor->getR(), fontColor->getG(), fontColor->getB()));

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring w = converter.from_bytes(text);

        graphics.DrawString(w.c_str(), -1, &font, rectF, &stringFormat, &solidBrush);
    }

protected:
};

} // namespace EBCpp

#endif