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
#include "renderer/EBGuiImage.hpp"
#include "renderer/EBGuiRenderImage.hpp"

namespace EBCpp
{

class EBGuiPicture : public EBGuiWidget
{
public:
    EBGuiPicture()
    {
    }

    void setImage(std::string filename)
    {
        image.load(filename);
        invalidate();
    }

protected:
    virtual void draw(std::list<EBObjectPointer<EBGuiRenderer>>& list)
    {
        EBObjectPointer<EBGuiWidget> p = parentWidget();
        int px = p->getX();
        int py = p->getY();

        EBObjectPointer<EBGuiImage> imgp = image.cast<EBGuiImage>();

        list.push_back(
        EBCpp::EBObjectBase::createObject<EBGuiRenderRect>(x + px, y + py, w, h, EB_COLOR_BLACK)->cast<EBGuiRenderer>());

        list.push_back(
        EBCpp::EBObjectBase::createObject<EBGuiRenderImage>(x + px, y + py, w, h, imgp)->cast<EBGuiRenderer>());
    }

private:
    EBGuiImage image;
};

} // namespace EBCpp
