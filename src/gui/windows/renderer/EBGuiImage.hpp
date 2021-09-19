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

    virtual void load( std::string filename )
    {
        /// TODO: if pointer != nullptr release memory

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring w = converter.from_bytes(filename);
        Gdiplus::Bitmap bmp(w.c_str());
        Gdiplus::BitmapData bitmapData;
        Gdiplus::Rect rect(0, 0, bmp.GetWidth(), bmp.GetHeight());

        width = bmp.GetWidth();
        height = bmp.GetHeight();

        if (Gdiplus::Ok ==
            bmp.LockBits(
            &rect, // A rectangle structure that specifies the portion of the Bitmap to lock.
            Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, // ImageLockMode values that specifies the access
                                                                      // level (read/write) for the Bitmap.
            bmp.GetPixelFormat(), // PixelFormat values that specifies the data format of the Bitmap.
            &bitmapData           // BitmapData that will contain the information about the lock operation.
            ))
        {
            switch (bmp.GetPixelFormat() )
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

            imagePointer = static_cast<uint8_t*>(malloc(len));
            memcpy(imagePointer, bitmapData.Scan0, len);

            bmp.UnlockBits(&bitmapData);
        }
        else
        {
            EB_LOG_DEBUG(EBLogger::LOG_DEBUG, "Can not load file " << filename);
        }
    }
};

} // namespace EBCpp
