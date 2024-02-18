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
#include "EBGuiColor.hpp"
#include "EBGuiPadding.hpp"

namespace EBCpp
{

class EBGuiVerticalLayout : public EBGuiWidget
{
public:
    EBGuiVerticalLayout() : EBGuiWidget(), padding({0, 0, 0, 0}), cellPadding({0, 0, 0, 0})
    {
    }

    virtual void prepare(int x, int y, int w, int h)
    {
        int height =
        this->h - (padding.top + padding.bottom + (cellPadding.top + cellPadding.bottom) * elements.size());

        int elementSum = 0;
        for( int i : elements )
        {
            elementSum += i;
        }
        float factor = (float)height / (float)elementSum;

        int i = 0;
        int currentY = padding.top;
        for (EBObjectPointer < EBGuiWidget> widget : widgets)
        {
            int size = 1;
            if (elements.size() > i)
            {
                std::list<int>::iterator it = std::next(elements.begin(), i);
                size = *it;
            }

            currentY += cellPadding.top;
            widget->setX(padding.left + cellPadding.bottom);
            widget->setY(currentY);
            widget->setWidth(this->w - (padding.left + padding.right + cellPadding.left + cellPadding.right));

            if( widget == widgets.back() )
            {
                widget->setHeight(height - currentY + cellPadding.bottom);
            }
            else
            {
                widget->setHeight(size * factor);
            }

            currentY += widget->getHeight();
            currentY += cellPadding.bottom;

            i++;
        }

        EBGuiWidget::prepare(x, y, w, h);
    }

    virtual void addElement( int elementSize )
    {
        elements.push_back(elementSize);
    }

    virtual void setPadding( EBGuiPadding & padding )
    {
        this->padding = padding;
        invalidate();
    }

    virtual void setCellPadding(EBGuiPadding& padding)
    {
        this->cellPadding = padding;
        invalidate();
    }

protected:
    virtual void draw(std::list<EBGuiRenderer*>& list)
    {
        // Layout is invisible
    }

private:
    std::list<int> elements;

    EBGuiPadding padding;
    EBGuiPadding cellPadding;
};

} // namespace EBCpp
