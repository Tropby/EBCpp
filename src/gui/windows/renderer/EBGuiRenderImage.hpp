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

#ifdef __WIN32__

namespace EBCpp
{

class EBGuiRenderImage : public EBGuiRenderImageBase
{
public:
    EBGuiRenderImage(int x, int y, int w, int h, EBObjectPointer<EBGuiImage>& image) :
        EBGuiRenderImageBase(x, y, w, h, image)
    {
    }

    virtual void render(Gdiplus::Graphics& graphics)
    {
        /// TODO: Handle other pixel formats
        Gdiplus::PixelFormat format = PixelFormat32bppARGB;
        switch( image->getPixelFormat() )
        {
            case EBGuiImage::PixelFormat24Bpp:
                format = PixelFormat24bppRGB;
                break;
        }

        Gdiplus::Bitmap bmp(image->getWidth(), image->getHeight(), image->getWidth() * image->getBytePerPixel(), format,
                            image->getRawData());

        float sx, sy;
        sx = (float)w / image->getWidth();
        sy = (float)h / image->getHeight();

        graphics.ScaleTransform(sx, sy);
        graphics.DrawImage(&bmp, x / sx, y / sy);
        graphics.ScaleTransform(1 / sx, 1 / sy);
    }

protected:
};

} // namespace EBCpp

#endif