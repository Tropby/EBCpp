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
#include "renderer/EBGuiColor.hpp"

namespace EBCpp
{

class EBGuiLabel : public EBGuiWidget
{
public:
    EBGuiLabel(EBObject* parent) : EBGuiWidget(parent), color(EB_COLOR_BLACK)
    {
    }

    void setText( std::string text )
    {
        this->text = text;
        invalidate();
    }

    void setColor( EBGuiColor & color )
    {
        this->color = color;
    }

protected:
    virtual void draw(std::list<EBGuiRenderer*> & list)
    {
        EBGuiWidget* p = parentWidget();
        int px = p->getX();
        int py = p->getY();

        list.push_back(new EBGuiRenderText(this, x + px, y + py, w, h, text, color));
    }

private:
    std::string text;
    EBGuiColor color;
};

} // namespace EBCpp
