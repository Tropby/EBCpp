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

class EBGuiHorizontalLayout : public EBGuiWidget
{
public:
    EBGuiHorizontalLayout() : EBGuiWidget(), padding({0,0,0,0}), cellPadding({0,0,0,0})
    {
    }

    virtual void prepare(int x, int y, int w, int h)
    {
        int width = this->w - ( padding.left + padding.right + ( cellPadding.left + cellPadding.right ) * elements.size() );

        int elementSum = 0;
        for( int i : elements )
        {
            elementSum += i;
        }
        float factor = (float)width / (float)elementSum;

        int i = 0;
        int currentX = padding.left;
        for (auto widget : widgets)
        {
            int size = 1;
            if (elements.size() > i)
            {
                std::list<int>::iterator it = std::next(elements.begin(), i);
                size = *it;
            }

            currentX += cellPadding.left;
            widget.get()->setX(currentX);
            widget.get()->setY( padding.top + cellPadding.top );
            widget.get()->setWidth(size * factor);
            widget.get()->setHeight(this->h - ( padding.top + padding.bottom + cellPadding.top + cellPadding.bottom ) );

            currentX += widget.get()->getWidth();
            currentX += cellPadding.right;

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
