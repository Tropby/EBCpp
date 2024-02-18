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

#include <codecvt>
#include <gdiplus.h>
#include <locale>

#include "../../profile/EBLogger.hpp"

namespace EBCpp
{

class EBImage : public EBImageBase
{
public:
    EBImage(std::string filename) : EBImageBase(filename)
    {
        load(filename);
    }

    EBImage() : EBImageBase()
    {
    }

    virtual bool load(EBString filename)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring w = converter.from_bytes(filename.toStdString());

        Gdiplus::Bitmap bmp(w.c_str());
        Gdiplus::GpStatus s = bmp.GetLastStatus();
        if( s != Gdiplus::GpStatus::Ok )
        {
            MessageBoxA(0, (EBString("Can not open file State: ") + std::to_string(s) ).dataPtr(),  "", 0);
            EB_LOG_CRITICAL("Can not open file State: " << s << "!");
            return false;
        }

        Gdiplus::BitmapData bitmapData;
        Gdiplus::Rect rect(0, 0, bmp.GetWidth(), bmp.GetHeight());

        width = bmp.GetWidth();
        height = bmp.GetHeight();

        Gdiplus::GpStatus d = bmp.LockBits(
            &rect, // A rectangle structure that specifies the portion of the Bitmap to lock.
            Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, // ImageLockMode values that specifies the access
                                                                    // level (read/write) for the Bitmap.
            bmp.GetPixelFormat(), // PixelFormat values that specifies the data format of the Bitmap.
            &bitmapData           // BitmapData that will contain the information about the lock operation.
        );

        if (Gdiplus::Ok == d)
        {
            switch (bmp.GetPixelFormat())
            {
            case PixelFormat24bppRGB:
                pixelFormat = PixelFormat24Bpp;
                break;
            case PixelFormat16bppRGB555:
                pixelFormat = PixelFormat16Bpp;
                break;
            case PixelFormat32bppARGB:
                pixelFormat = PixelFormat32Bpp;
                break;
            default:
                pixelFormat = PixelFormatUnknown;
                break;
            }

            bpp = bitmapData.Stride / width;

            int len = bitmapData.Height * std::abs(bitmapData.Stride);

            imagePointer = new uint8_t[len];
            memcpy(imagePointer, bitmapData.Scan0, len);

            bmp.UnlockBits(&bitmapData);
        }
        else
        {
            EB_LOG_DEBUG("Can not load file " << filename);            
            return false;
        }
        return true;
    }
};

} // namespace EBCpp