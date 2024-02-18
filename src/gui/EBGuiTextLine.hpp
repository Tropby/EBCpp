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
#include "../EBTimer.hpp"
#include "EBGuiWidget.hpp"
#include "EBGuiColor.hpp"

namespace EBCpp
{

class EBGuiTextLine : public EBGuiWidget
{
public:
    EBGuiTextLine() :
        EBGuiWidget(), textColor(EB_COLOR_BLACK), timer(EBCpp::EBObjectBase::createObject<EBTimer>()), cursorPos(0)
    {
        timer->timeout.connect(this, &EBGuiTextLine::timeout);
        timer->startSingleShot(50);
    }

    void setText(std::string text)
    {
        this->text = text;
    }

    EBString getText() const
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

protected:
    virtual void mouseLeave(int x, int y)
    {
        // Todo: This is only working on windows
        HCURSOR hc = LoadCursor(hInstance, IDC_ARROW);
        SetCursor(hc);

        bool mid = mouseIsDown;
        mouseIsDown = false;
        if (mid)
            invalidate();
    }

    virtual void mouseHover(int x, int y)
    {        
        // Todo: This is only working on windows
        HCURSOR hc = LoadCursor(hInstance, IDC_IBEAM);
        SetCursor(hc);
    }

    virtual bool mouseDown(int x, int y)
    {
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
            setFocus();
        }

        mouseIsDown = false;

        if (mid)
            invalidate();

        return true;
    }

    virtual void specialKeyPress(int key)
    {
        std::string preText = text.substr(0, cursorPos);
        std::string sufText = text.substr(cursorPos);

        if( key == 0x08 )
        {
            if (cursorPos > 0 )
            {
                text = preText.substr(0, preText.length() - 1) + sufText;
                cursorPos--;
            }
        }
        else if( key == 37)
        {
            if( cursorPos > 0 )
                cursorPos--;
        }
        else if( key == 39)
        {
            if( cursorPos < text.length() )            
                cursorPos++;
        }
        else if( key == 46)
        {
            if (cursorPos < text.length() )
            {
                text = preText + sufText.substr(1);
            }
        }
        invalidate();
    } 

    virtual void keyPress(char key)
    {
        std::string preText = text.substr(0, cursorPos);
        std::string sufText = text.substr(cursorPos);

        if( key != 0x08)
        {
            text = preText + key + sufText;
            cursorPos++;
            invalidate();
        }
    }

    virtual void draw(EBGuiRenderer& renderer)
    {
        EBGuiWidget::draw(renderer);

        EBObjectPointer<EBGuiWidget> p = parentWidget();

        if (isFocused())
        {
            renderer.drawTextWithCursor(0, 0, w, h, text, cursorPos, textColor);
        }
        else
        {
            renderer.drawText(1, 1, w, h, text, textColor);
        }
    }

private:

    EB_SLOT(timeout)
    {
        timer->startSingleShot(120);
        invalidate();
    }

    bool mouseIsDown;

    EBObjectPointer<EBTimer> timer;

    std::string text;
    int cursorPos;
    EBObjectPointer < EBGuiColor > textColor;
};

} // namespace EBCpp
