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

#include "EBGuiColor.hpp"
#include "EBGuiWidget.hpp"

namespace EBCpp
{

class EBGuiLabel : public EBGuiWidget
{
public:
    EBGuiLabel() : EBGuiWidget(), color(EB_COLOR_BLACK)
    {
    }

    void setText(EBString text)
    {
        this->text = text.toStdString();
        invalidate();
    }

    void setTextHorizontalAlignment(EBGuiHorizontalAlignment horizontalAlignment)
    {
        this->horizontalAlignment = horizontalAlignment;
        invalidate();
    }

    void setTextVerticalAlignment(EBGuiVerticalAlignment verticalAlignment)
    {
        this->verticalAlignment = verticalAlignment;
        invalidate();
    }

    void setColor(EBObjectPointer<EBGuiColor> color)
    {
        this->color = color;
    }

protected:
    virtual void draw(EBGuiRenderer& renderer)
    {
        EBGuiWidget::draw(renderer);
        renderer.drawText(0, 0, w, h, text, color, horizontalAlignment, verticalAlignment);
    }

private:
    std::string text;
    EBObjectPointer<EBGuiColor> color;
    EBGuiHorizontalAlignment horizontalAlignment = EBGuiHorizontalAlignment::EB_HOR_ALIGN_LEFT;
    EBGuiVerticalAlignment verticalAlignment = EBGuiVerticalAlignment::EB_VERT_ALIGN_CENTER;
};

} // namespace EBCpp
