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

namespace EBCpp
{

#include "../../EBObject.hpp"

class EBGuiImageBase : public EBObject<EBGuiImageBase>
{
public:
    EBGuiImageBase() : imagePointer(nullptr), width(0), height(0), bpp(0), pixelFormat(PixelFormatUnknown)
    {
    }

    EBGuiImageBase(std::string filename) :
        imagePointer(nullptr), width(0), height(0), bpp(0), pixelFormat(PixelFormatUnknown)
    {
    }

    virtual bool load(EBString filename) = 0;

    enum PIXEL_FORMAT
    {
        PixelFormat8bpp = 1,
        PixelFormat16Bpp = 2,
        PixelFormat24Bpp = 3,
        PixelFormat32Bpp = 4,
        PixelFormatUnknown = -1
    };

    uint8_t* getRawData()
    {
        return imagePointer;
    }

    int getWidth()
    {
        return width;
    }
    
    int getHeight()
    {
        return height;
    }

    PIXEL_FORMAT getPixelFormat()
    {
        return pixelFormat;
    }

    int getBytePerPixel()
    {
        return bpp;
    }

protected:
    int width;
    int height;
    PIXEL_FORMAT pixelFormat;
    int bpp;

    uint8_t* imagePointer;
};

} // namespace EBCpp

#ifdef __WIN32__
#include "../windows/renderer/EBGuiImage.hpp"
#endif
