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

#include "../EBObject.hpp"
#include "../EBList.hpp"
#include "EBGuiColor.hpp"
#include "renderer/EBGuiRenderTextLine.hpp"
#include "renderer/EBGuiRenderTextLineWithCursor.hpp"
#include "EBGuiRenderer.hpp"

namespace EBCpp
{

class EBGuiWidget : public EBObject<EBGuiWidget>
{
public:
    EBGuiWidget() :
        EBObject(), visible(true), widgetParent(nullptr), w(100), h(100), x(0), y(0), minW(0), minH(0), maxW(INT_MAX),
        maxH(INT_MAX), backgroundColor(EB_COLOR_WHITE), borderColor(EB_COLOR_BLACK)
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
        for (const auto& widget : widgets)
        {
            widget.get()->prepare(x, y, w, h);
        }
    }

    virtual void invalidate()
    {
        if (this->parentWidget().get() != nullptr)
            this->parentWidget()->invalidate();
    }

    void render(EBGuiRenderer& renderer)
    {
        EBGuiRenderer widgetRenderer(getX(), getY(), getWidth(), getHeight(), renderer.getHDC());

        // Create drawing for the current widget
        draw(widgetRenderer);

        // Create drawings for all child widgets
        for (const auto& w : widgets)
        {
            if( visible )
                w.get()->render(widgetRenderer);
        }

        widgetRenderer.transfer(renderer.getHDC());
    }

    virtual void show()
    {
        visible = true;
        invalidate();
    }

    virtual void hide()
    {
        visible = false;
        invalidate();
    }

    virtual void setX(int x)
    {
        this->x = x;
        invalidate();
    }

    virtual int getX()
    {
        return x;
    }

    virtual void setY(int y)
    {
        this->y = y;
        invalidate();
    }

    virtual int getY()
    {
        return y;
    }

    virtual int getWidth()
    {
        return w;
    }

    virtual int getHeight()
    {
        return h;
    }

    virtual void setMinWidth(int width)
    {
        minW = width;
        if (w < width)
        {
            w = width;
            invalidate();
        }
    }

    virtual void setMinHeight(int height)
    {
        minH = height;
        if (h < height)
        {
            h = height;
            invalidate();
        }
    }

    virtual void setMaxWidth(int width)
    {
        maxW = width;
        if (w > width)
        {
            w = width;
            invalidate();
        }
    }

    virtual void setMaxHeight(int height)
    {
        maxH = height;
        if (h > height)
        {
            h = height;
            invalidate();
        }
    }

    virtual void setWidth(int width)
    {
        if (width > maxW)
            width = maxW;
        if (width < minW)
            width = minW;

        if (this->w != width)
            invalidate();
        this->w = width;
    }

    virtual void setHeight(int height)
    {
        if (height > maxH)
            height = maxH;
        if (height < minH)
            height = minH;

        if (this->h != height)
            invalidate();
        this->h = height;
    }

    virtual void addWidget(EBObjectPointer<EBGuiWidget> widget)
    {
        if (widget->widgetParent != nullptr)
            widget->widgetParent->removeWidget(widget);
        widget->widgetParent = &(*this);
        widgets.append(widget);
    }

    virtual void addWidget(EBGuiWidget& widget)
    {
        EBObjectPointer<EBGuiWidget> widgetPointer = &widget;
        if (widgetPointer->widgetParent != nullptr)
            widgetPointer->widgetParent->removeWidget(widgetPointer);
        widgetPointer->widgetParent = &(*this);
        widgets.append(widgetPointer);
    }

    virtual void removeWidget(EBObjectPointer<EBGuiWidget> widget)
    {
        widgets.remove(widget);
    }

    EBObjectPointer<EBGuiWidget> parentWidget()
    {
        return this->widgetParent;
    }

    bool handleMouseDown(int x, int y)
    {
        if (!mouseInWidget)
            return false;

        for (const auto& w : widgets)
        {
            if (w.get()->handleMouseDown(x, y))
            {
                // If one component have handled the click event quit the loop
                return true;
            }
        }

        return mouseDown(x, y);
    }

    void handleKeyPress(char key)
    {
        if (this->isFocused())
        {
            keyPress(key);
            return;
        }

        for (const auto & w : widgets)
        {
            w.get()->handleKeyPress(key);
        }
    }

    bool handleMouseUp(int x, int y)
    {
        if (!mouseInWidget)
            return false;

        for (const auto& w : widgets)
        {
            if (w.get()->handleMouseUp(x, y))
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

        if (this->parentWidget() != nullptr)
        {
            px = this->parentWidget()->getX();
            py = this->parentWidget()->getY();
        }

        // test if mouse is in widget
        mouseInWidget =
        (this->x + px <= x) && (this->x + px + this->w >= x) && (this->y + py <= y) && (this->y + py + this->h >= y);

        if (mouseInWidget && !changed)
            mouseHover(x, y);
        else if (!mouseInWidget && changed)
            mouseLeave(x, y);

        changed = (mouseInWidget != changed);

        for (const auto& w : widgets)
        {
            changed |= w.get()->setMousePos(x, y);
        }

        return changed;
    }

    bool isFocused()
    {
        return focused;
    }

    void setFocus()
    {
        EBObjectPointer<EBGuiWidget>& root = this->widgetParent;

        // Update Focus for all widgets
        while (root->widgetParent != nullptr)
        {
            root = root->widgetParent;
        }
        root->clearFocus();
        this->focused = true;
    }

    void clearFocus()
    {
        this->focused = false;
        for (const auto& widget : widgets)
        {
            widget.get()->clearFocus();
        }
    }

    virtual void setBorderColor(const EBObjectPointer<EBGuiColor> borderColor)
    {
        this->borderColor = borderColor;
        invalidate();
    }

    virtual void setBackgroundColor(const EBObjectPointer<EBGuiColor> backgroundColor)
    {
        this->backgroundColor = backgroundColor;
        invalidate();
    }

protected:
    virtual void keyPress(char key)
    {
    }

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

    virtual void draw(EBGuiRenderer & renderer)
    {
        EBObjectPointer<EBGuiWidget> p = parentWidget();
        int px = 0;
        int py = 0;

        if (p != nullptr)
        {
            px = p->getX();
            py = p->getY();
        }

        renderer.drawFillRect(0, 0, w, h, backgroundColor);
        renderer.drawRect(0, 0, w-1, h-1, borderColor);
    }

    bool mouseInWidget;
    bool visible;

    bool focused;

    int x;
    int y;
    int w;
    int h;
    int minW;
    int minH;
    int maxW;
    int maxH;

    EBList<EBObjectPointer<EBGuiWidget>> widgets;
    EBObjectPointer<EBGuiColor> borderColor;
    EBObjectPointer<EBGuiWidget> widgetParent;
    EBObjectPointer<EBGuiColor> backgroundColor;
};

} // namespace EBCpp

