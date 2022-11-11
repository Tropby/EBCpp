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

#include "EBGuiWidget.hpp"

namespace EBCpp
{

class EBGuiWindowBase : public EBGuiWidget
{    
public:
    EBGuiWindowBase() : EBGuiWidget()
    {
    }

protected : 

    struct {
        int x;
        int y;
    } mousePos;

    virtual bool setMousePos( int x, int y )
    {
        mousePos.x = x;        
        mousePos.y = y;
        return EBGuiWidget::setMousePos(x, y);
    }

    virtual void setTitle(std::string title) = 0;

    virtual void resized(int w, int h)
    {
        if(this->widgets.getSize() > 0 )
        {
            EBObjectPointer<EBGuiWidget> widget = this->widgets.get(0);
            widget->setX(0);
            widget->setY(0);
            widget->setWidth(w);
            widget->setHeight(h);
        }

        invalidate();
    }
};

} // namespace EBCpp

#ifdef __WIN32__
    #include "windows/EBGuiWindow.hpp"
#endif