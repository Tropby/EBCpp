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

#include "EBGuiRenderTextLineWithCursor.hpp"
#include "../../renderer/EBGuiRect.hpp"
#include "../../../EBUtils.hpp"
#include <codecvt>
#include <locale>

#ifdef __WIN32__

namespace EBCpp
{

class EBGuiRenderTextLineWithCursor : public EBGuiRenderTextLineWithCursorBase
{
public:
    EBGuiRenderTextLineWithCursor(int x, int y, int maxWidth, std::string text,
                                  int cursorPos, EBObjectPointer < EBGuiColor> fontColor = EB_COLOR_BLACK) :
        EBGuiRenderTextLineWithCursorBase(x, y, maxWidth, text, cursorPos, fontColor)
    {
    }

    virtual void render(Gdiplus::Graphics& graphics)
    {
        Gdiplus::FontFamily fontFamily(L"Times New Roman");
        Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::PointF pointF(x, y);
        Gdiplus::StringFormat stringFormat;

        Gdiplus::SolidBrush solidBrush(
        Gdiplus::Color(fontColor->getA(), fontColor->getR(), fontColor->getG(), fontColor->getB()));

        std::string preText = text.substr(0, cursorPos);
        std::string sufText = text.substr(cursorPos);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring preTextw = converter.from_bytes(preText);
        std::wstring sufTextw = converter.from_bytes(sufText);

        // Measure Text size
        Gdiplus::RectF outRect;
        Gdiplus::Region regionsList[1];
        Gdiplus::CharacterRange ranges[1];
        Gdiplus::RectF layoutRect(0, 0, 1000, 100);
        ranges[0].First = 0;
        ranges[0].Length = preText.length();
        stringFormat.SetMeasurableCharacterRanges(1, ranges);
        std::replace(preText.begin(), preText.end(), ' ', '.');
        if (graphics.MeasureCharacterRanges(converter.from_bytes(preText).c_str(), -1, &font, layoutRect,
                                                                          &stringFormat, 1, regionsList) != Gdiplus::Ok)
        {
            outRect.Width = 0;
            outRect.Height = 24;
        }
        else
        {
            if (regionsList[0].GetBounds(&outRect, &graphics) != Gdiplus::Ok)
            {
                outRect.Width = 0;
                outRect.Height = 24;
            }
        }

        // Draw String bevor Cursor!
        graphics.DrawString(preTextw.c_str(), -1, &font, pointF, &stringFormat, &solidBrush);

        Gdiplus::Pen pen(Gdiplus::Color(fontColor->getA(), fontColor->getR(), fontColor->getG(), fontColor->getB()));
        pointF.X = pointF.X + outRect.Width + 5;

        Gdiplus::PointF pointCursor1(pointF.X, y + 4);
        Gdiplus::PointF pointCursor2(pointF.X, y + outRect.Height - 8);

        if( EBUtils::uptime() % 1000 > 500 )
            graphics.DrawLine(&pen, pointCursor1, pointCursor2);

        graphics.DrawString(sufTextw.c_str(), -1, &font, pointF, &stringFormat, &solidBrush);
    }

protected:
};

} // namespace EBCpp

#endif