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

    void setText(std::string text)
    {
        this->text = text;
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
