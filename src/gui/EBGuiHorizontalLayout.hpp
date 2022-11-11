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
