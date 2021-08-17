/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
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

#include "../EBObject.hpp"
#include "renderer/EBGuiRenderRect.hpp"
#include "renderer/EBGuiRenderText.hpp"
#include "renderer/EBGuiRenderer.hpp"

namespace EBCpp
{

class EBGuiWidget : public EBObject
{
public:
    EBGuiWidget(EBObject* parent) : EBObject(parent), visible(false), widgetParent(nullptr)
    {
    }

    /**
     * @brief Update the widget size if used for responsible layouts
     *
     * @param x
     * @param y
     * @param w
     * @param h
     */
    virtual void prepare(int x, int y, int w, int h)
    {
        for (EBGuiWidget* widget : widgets)
        {
            widget->prepare(x, y, w, h);
        }
    }

    virtual void invalidate()
    {
        if( this->parentWidget() )
            this->parentWidget()->invalidate();
    }

    void render(std::list<EBGuiRenderer*>& list)
    {
        draw(list);
        for (EBGuiWidget* w : widgets)
        {
            w->render(list);
        }
    }

    virtual void show()
    {
        visible = true;
    }

    virtual void hide()
    {
        visible = false;
    }

    virtual void setX(int x)
    {
        this->x = x;
    }

    virtual int getX()
    {
        return x;
    }

    virtual int getY()
    {
        return y;
    }

    virtual void setY(int y)
    {
        this->y = y;
    }

    virtual void setWidth(int width)
    {
        this->w = width;
    }

    virtual void setHeight(int height)
    {
        this->h = height;
    }

    virtual void addWidget(EBGuiWidget* widget)
    {
        if (widget->widgetParent != nullptr)
            widget->widgetParent->removeWidget(widget);
        widget->widgetParent = this;
        widgets.push_back(widget);
    }

    virtual void removeWidget(EBGuiWidget* widget)
    {
        widgets.remove(widget);
    }

    EBGuiWidget* parentWidget()
    {
        return this->widgetParent;
    }

    bool handleMouseDown(int x, int y)
    {
        if( !mouseInWidget )
            return false;

        for( EBGuiWidget * w : widgets )
        {
            if( w->handleMouseDown(x, y) )
            {
                // If one component have handled the click event quit the loop
                return true;
            }
        }

        return mouseDown(x, y);
    }

    bool handleMouseUp(int x, int y)
    {
        if (!mouseInWidget)
            return false;

        for (EBGuiWidget* w : widgets)
        {
            if (w->handleMouseUp(x, y))
            {
                // If one component have handled the click event quit the loop
                return true;
            }
        }
        return mouseUp(x, y);
    }

    bool setMousePos(int x, int y)
    {
        bool changed = mouseInWidget;
        int px = 0;
        int py = 0;

        if( this->parentWidget() != nullptr )
        {
            px = this->parentWidget()->getX();
            py = this->parentWidget()->getY();
        }

        // test if mouse is in widget
        mouseInWidget =
            (this->x + px <= x) && (this->x + px + this->w >= x) &&
            (this->y + py <= y) && (this->y + py + this->h >= y);

        if (mouseInWidget && !changed)
            mouseHover(x, y);
        else if( !mouseInWidget && changed )
            mouseLeave(x, y);

        changed = (mouseInWidget != changed);

        for (EBGuiWidget* w : widgets)
        {
            changed |= w->setMousePos(x, y);
        }

        return changed;
    }

protected:
    virtual void mouseLeave(int x, int y)
    {
    }

    virtual void mouseHover(int x, int y)
    {
    }

    virtual bool mouseDown(int x, int y)
    {
        return false;
    }

    virtual bool mouseUp(int x, int y)
    {
        return false;
    }

    virtual void draw(std::list<EBGuiRenderer*>& list)
    {
        EBGuiRenderRect* rect = new EBGuiRenderRect(this, x, y, x + w, y + h);
        list.push_back(rect);
    }

    bool mouseInWidget;
    bool visible;
    int x;
    int y;
    int w;
    int h;
    std::list<EBGuiWidget*> widgets;
    EBGuiWidget* widgetParent;
};

} // namespace EBCpp
