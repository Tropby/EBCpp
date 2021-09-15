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

#include "../EBEvent.hpp"
#include "../EBTimer.hpp"
#include "EBGuiWidget.hpp"
#include "renderer/EBGuiColor.hpp"

namespace EBCpp
{

class EBGuiTextLine : public EBGuiWidget
{
public:
    EBGuiTextLine() :
        EBGuiWidget(), textColor(EB_COLOR_BLACK), borderColor(EB_COLOR_BLACK), timer(EBCpp::EBObjectBase::createObject<EBTimer>()), cursorPos(0)
    {
        timer->timeout.connect(this, &EBGuiTextLine::timeout);
        timer->startSingleShot(50);
    }

    void setText(std::string text)
    {
        this->text = text;
    }

    std::string getText()
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

    virtual void keyPress(char key)
    {
        printf("Text update %X\n", key);
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
        else
        {
            text = preText + key + sufText;
            cursorPos++;
        }

        invalidate();
    }

    virtual void draw(std::list< EBObjectPointer<EBGuiRenderer> >& list)
    {
        EBObjectPointer<EBGuiWidget> p = parentWidget();
        int px = p->getX();
        int py = p->getY();        
        list.push_back( EBCpp::EBObjectBase::createObject< EBGuiRenderRect>( x + px, y + py, w, h, borderColor)->cast<EBGuiRenderer>());
        list.push_back(
        EBCpp::EBObjectBase::createObject<EBGuiRenderRect>(x + px + 1, y + py + 1, w - 1, h - 1, borderColor)
        ->cast<EBGuiRenderer>());

        if (isFocused())
        {
            list.push_back(
            EBCpp::EBObjectBase::createObject<EBGuiRenderTextLineWithCursor>(x + px, y + py, w, text, cursorPos)
            ->cast<EBGuiRenderer>());
        }
        else
        {
            list.push_back(
            EBCpp::EBObjectBase::createObject<EBGuiRenderTextLine>(x + px, y + py, w, text)->cast<EBGuiRenderer>());
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
    EBObjectPointer < EBGuiColor > borderColor;
};

} // namespace EBCpp
