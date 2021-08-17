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

#include "../src/EBEventLoop.hpp"
#include "../src/EBObject.hpp"
#include "../src/gui/EBGuiLabel.hpp"
#include "../src/gui/EBGuiWidget.hpp"
#include "../src/gui/EBGuiWindow.hpp"
#include "../src/gui/EBGuiButton.hpp"

#include <iostream>

class ExampleGui : public EBCpp::EBObject
{
public:
    ExampleGui() : EBObject(nullptr), window(this)
    {
        window.closed.connect(*this, &ExampleGui::windowClosed);
        EBCpp::EBGuiWidget* widget = new EBCpp::EBGuiWidget(&window);
        window.addWidget(widget);

        widget->setX(20);
        widget->setY(20);
        widget->setWidth(800);
        widget->setHeight(600);

        EBCpp::EBGuiLabel* label = new EBCpp::EBGuiLabel(widget);
        widget->addWidget(label);
        label->setX(20);
        label->setY(20);
        label->setWidth(200);
        label->setHeight(40);
        label->setText("Hallo Welt!");

        EBCpp::EBGuiButton* button = new EBCpp::EBGuiButton(widget);
        widget->addWidget(button);
        button->setX(20);
        button->setY(100);
        button->setWidth(200);
        button->setHeight(40);
        button->setText("Hallo Welt!");
    }

private:
    EBCpp::EBGuiWindow window;    

    EB_SLOT(windowClosed)
    {        
        EBCpp::EBEventLoop::getInstance().exit();
    }
};

/**
 * @brief Main from the timer example
 *
 * @return int Exit code (0)
 */
int main()
{
    ExampleGui gui;
    EBCpp::EBEventLoop::getInstance().exec();
    return 0;
}