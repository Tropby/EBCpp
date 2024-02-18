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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "../../EBObject.hpp"
#include "../../EBOs.hpp"
#include "../EBGuiAlignment.hpp"
#include "../EBGuiColor.hpp"
#include "../EBImage.hpp"
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

    virtual void drawTextWithCursor(int x, int y, int w, int h, EBString text, int cursorPosition,
                                    const EBObjectPointer<EBGuiColor>& fontColor)
    {
        Gdiplus::FontFamily fontFamily(L"Arial");
        Gdiplus::Font font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::PointF pointF(x, y);
        Gdiplus::StringFormat stringFormat;

        Gdiplus::SolidBrush solidBrush(
        Gdiplus::Color(fontColor->getA(), fontColor->getR(), fontColor->getG(), fontColor->getB()));

        std::string preText = text.mid(0, cursorPosition).toStdString();
        std::string sufText = text.mid(cursorPosition).toStdString();

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
        if (graphics->MeasureCharacterRanges(converter.from_bytes(preText).c_str(), -1, &font, layoutRect, &stringFormat,
                                            1, regionsList) != Gdiplus::Ok)
        {
            outRect.Width = 0;
            outRect.Height = 24;
        }
        else
        {
            if (regionsList[0].GetBounds(&outRect, graphics) != Gdiplus::Ok)
            {
                outRect.Width = 0;
                outRect.Height = 24;
            }
        }

        // Draw String bevor Cursor!
        graphics->DrawString(preTextw.c_str(), -1, &font, pointF, &stringFormat, &solidBrush);

        Gdiplus::Pen pen(Gdiplus::Color(fontColor->getA(), fontColor->getR(), fontColor->getG(), fontColor->getB()));
        pointF.X = pointF.X + outRect.Width;

        Gdiplus::PointF pointCursor1(pointF.X + 4, y + 2);
        Gdiplus::PointF pointCursor2(pointF.X + 4, y + outRect.Height - 2);

        if (EBUtils::uptime() % 1000 > 500)
            graphics->DrawLine(&pen, pointCursor1, pointCursor2);

        graphics->DrawString(sufTextw.c_str(), -1, &font, pointF, &stringFormat, &solidBrush);
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

    virtual void drawImage(int x, int y, int w, int h, EBObjectPointer<EBImageBase>& image)
    {
        /// TODO: Handle other pixel formats
        Gdiplus::PixelFormat format = PixelFormat32bppARGB;
        switch( image->getPixelFormat() )
        {
            case EBImage::PixelFormat24Bpp:
                format = PixelFormat24bppRGB;
                break;
        }

        Gdiplus::Bitmap bmp(image->getWidth(), image->getHeight(), image->getWidth() * image->getBytePerPixel(), format,
                            image->getRawData());

        float sx, sy;
        sx = (float)w / image->getWidth();
        sy = (float)h / image->getHeight();

        graphics->ScaleTransform(sx, sy);
        graphics->DrawImage(&bmp, x / sx, y / sy);
        graphics->ScaleTransform(1 / sx, 1 / sy);
    }

private:
    Gdiplus::Graphics* graphics;

    HDC hdcMem;
    HBITMAP MemBitmap;
    HBITMAP hbmOld;
};

} // namespace EBCpp

#endif