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

#include "EBGuiWidget.hpp"

namespace EBCpp
{

class EBGuiWindowBase : public EBGuiWidget
{    
public:
    EBGuiWindowBase() : EBGuiWidget()
    {
    }

    virtual void addWidget(EBObjectPointer<EBGuiWidget> widget)
    {
        EBGuiWidget::addWidget(widget);
        widget->setWidth(w);
        widget->setHeight(h);
        widget->setX(0);
        widget->setY(0);
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