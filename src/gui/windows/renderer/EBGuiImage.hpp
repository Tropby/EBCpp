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

#include <codecvt>
#include <gdiplus.h>
#include <locale>

#include "../../../profile/EBLogger.hpp"

namespace EBCpp
{

class EBGuiImage : public EBGuiImageBase
{
public:
    EBGuiImage(std::string filename) : EBGuiImageBase(filename)
    {
        load(filename);
    }

    EBGuiImage() : EBGuiImageBase()
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
