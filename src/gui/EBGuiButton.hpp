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

#include "../EBEvent.hpp"
#include "EBGuiWidget.hpp"
#include "EBGuiColor.hpp"

namespace EBCpp
{

class EBGuiButton : public EBGuiWidget
{
public:
    EBGuiButton() :
        EBGuiWidget(), textColor(EB_COLOR_BLACK), borderColor(EB_COLOR_BLACK), mouseIsDown(false)
    {
    }   

    void setText(EBString text)
    {
        this->text = text;
    }

    EBString getText()
    {
        return this->text;
    }

    void setTextColor(EBObjectPointer<EBGuiColor> textColor)
    {
        this->textColor = textColor;
    }

    void setBorderColor(EBObjectPointer<EBGuiColor> borderColor)
    {
        this->borderColor = borderColor;
    }

    EB_SIGNAL(clicked);

protected:
    virtual void mouseLeave(int x, int y)
    {
        bool mid = mouseIsDown;
        mouseIsDown = false;
        if (mid)
            invalidate();
    }

    virtual void mouseHover(int x, int y)
    {
        bool mid = mouseIsDown;
    }

    virtual bool mouseDown(int x, int y)
    {
        setFocus();

        bool mid = mouseIsDown;
        mouseIsDown = true;
        if (!mid)
            invalidate();
        return true;
    }

    virtual bool mouseUp(int x, int y)
    {
        bool mid = mouseIsDown;

        if (mouseIsDown)
        {
            EB_EMIT(clicked);
        }

        mouseIsDown = false;

        if (mid)
            invalidate();

        return true;
    }

    virtual void draw(EBGuiRenderer& renderer)
    {
        EBObjectPointer<EBGuiWidget> p = parentWidget();
        int px = p->getX();
        int py = p->getY();

        renderer.drawFillRect(0, 0, w, h, backgroundColor);
        renderer.drawText(0, 0, w, h, text, textColor, EB_HOR_ALIGN_CENTER, EB_VERT_ALIGN_CENTER);
        renderer.drawRect(0, 0, w - 2, h - 2, borderColor);

        if( mouseIsDown )
            renderer.drawRect(1, 1, w - 1, h - 1, borderColor);
        else
            renderer.drawRect(0, 0, w - 1, h - 1, borderColor);
    }

private:
    EBString text;
    EBObjectPointer<EBGuiColor> textColor;
    EBObjectPointer<EBGuiColor> borderColor;

    bool mouseIsDown;
};

} // namespace EBCpp
