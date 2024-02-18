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

namespace EBCpp
{

#include "../EBObject.hpp"

class EBImageBase : public EBObject<EBImageBase>
{
public:
    EBImageBase() : imagePointer(nullptr), width(0), height(0), bpp(0), pixelFormat(PixelFormatUnknown)
    {
    }

    EBImageBase(std::string filename) :
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
#include "windows/EBImage.hpp"
#endif
