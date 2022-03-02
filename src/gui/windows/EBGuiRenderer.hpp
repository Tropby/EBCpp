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

#include "../../EBObject.hpp"
#include "../../EBOs.hpp"
#include "../EBGuiAlignment.hpp"
#include "../EBGuiColor.hpp"
#include <string>

#ifdef __WIN32__

#include <codecvt>
#include <gdiplus.h>
#include <locale>

namespace EBCpp
{

class EBGuiRenderer : public EBGuiRendererBase
{
public:
    EBGuiRenderer(int x, int y, int w, int h, HDC hdc) : EBGuiRendererBase(x, y, w, h)
    {
        hdcMem = CreateCompatibleDC(hdc);
        MemBitmap = CreateCompatibleBitmap(hdc, w, h);
        SelectObject(hdcMem, MemBitmap);
        SetTextColor(hdcMem, GetSysColor(COLOR_WINDOWTEXT));
        this->graphics = new Gdiplus::Graphics(hdcMem);
    }

    ~EBGuiRenderer()
    {
        delete this->graphics;

        // Free Memory
        DeleteDC(hdcMem);
        DeleteObject(MemBitmap);
    }

    void transfer(HDC dest)
    {
        BitBlt(dest, getX(), getY(), getX() + getWidth(), getY() + getHeight(), hdcMem, 0, 0, SRCCOPY);
    }

    virtual HDC getHDC()
    {
        return hdcMem;
    }

    virtual void drawText(int x, int y, int w, int h, EBString text, const EBObjectPointer<EBGuiColor>& fontColor,
                          EBGuiHorizontalAlignment horizontalAlignment = EBGuiHorizontalAlignment::EB_HOR_ALIGN_LEFT,
                          EBGuiVerticalAlignment verticalAlignment = EBGuiVerticalAlignment::EB_VERT_ALIGN_TOP)
    {
        Gdiplus::FontFamily fontFamily(L"Arial");
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
        case EBGuiHorizontalAlignment::EB_HOR_ALIGN_UNCHANGED:
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
        case EBGuiVerticalAlignment::EB_VERT_ALIGN_UNCHANGED:
            break;
        }

        Gdiplus::SolidBrush solidBrush(
        Gdiplus::Color(fontColor->getA(), fontColor->getR(), fontColor->getG(), fontColor->getB()));

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(text.toStdString());

        graphics->DrawString(wstr.c_str(), -1, &font, rectF, &stringFormat, &solidBrush);
    }

    virtual void drawRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor>& borderColor)
    {
        Gdiplus::Pen pen(
        Gdiplus::Color(borderColor->getA(), borderColor->getR(), borderColor->getG(), borderColor->getB()));
        graphics->DrawRectangle(&pen, x, y, w, h);
    }

    virtual void drawFillRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor> backgroundColor)
    {
        Gdiplus::SolidBrush brush(Gdiplus::Color(backgroundColor->getA(), backgroundColor->getR(),
                                                 backgroundColor->getG(), backgroundColor->getB()));
        graphics->FillRectangle(&brush, x, y, w, h);
    }

private:
    Gdiplus::Graphics* graphics;

    HDC hdcMem;
    HBITMAP MemBitmap;
    HBITMAP hbmOld;   
};

} // namespace EBCpp

#endif