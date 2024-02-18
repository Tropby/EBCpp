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

#include "../EBString.hpp"
#include "EBGuiAlignment.hpp"
#include "EBGuiColor.hpp"
#include "EBImage.hpp"

namespace EBCpp
{

class EBGuiRendererBase : public EBObject<EBGuiRendererBase>
{
public:
    EBGuiRendererBase(int x, int y, int width, int height) : EBObject(), x(x), y(y), width(width), height(height)
    {
    }

    int getWidth()
    {
        return width;
    }

    int getHeight()
    {
        return height;
    }

    int getX()
    {
        return x;
    }

    int getY()
    {
        return y;
    }

    virtual void drawRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor>& borderColor) = 0;
    virtual void drawFillRect(int x, int y, int w, int h, EBObjectPointer<EBGuiColor> backgroundColor) = 0;

    virtual void drawText(int x, int y, int w, int h, EBString text, const EBObjectPointer<EBGuiColor>& fontColor,
                          EBGuiHorizontalAlignment horizontalAlignment = EBGuiHorizontalAlignment::EB_HOR_ALIGN_LEFT,
                          EBGuiVerticalAlignment verticalAlignment = EBGuiVerticalAlignment::EB_VERT_ALIGN_TOP) = 0;

    virtual void drawTextWithCursor(int x, int y, int w, int h, EBString text, int cursorPosition,
                                    const EBObjectPointer<EBGuiColor>& fontColor) = 0;

    virtual void drawImage(int x, int y, int w, int h, EBObjectPointer<EBImageBase>& image) = 0;
private:
    int x;
    int y;
    int width;
    int height;
};

} // namespace EBCpp

#ifdef __WIN32__
#include "windows/EBGuiRenderer.hpp"
#else
#warning "Can not use EBGuiRenderer on this platform!"
#endif
