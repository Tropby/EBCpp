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

    void setText(std::string text)
    {
        this->text = text;
    }

    void setText(EBString& text)
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
