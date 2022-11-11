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
#include "../src/gui/EBGuiButton.hpp"
#include "../src/gui/EBGuiLabel.hpp"
#include "../src/gui/EBGuiWidget.hpp"
#include "../src/gui/EBGuiWindow.hpp"
#include "../src/gui/EBGuiHorizontalLayout.hpp"
#include "../src/gui/EBGuiPadding.hpp"
#include "../src/gui/EBGuiTextLine.hpp"

#include <iostream>
#include <memory>

class ExampleGuiLayout : public EBCpp::EBObject < ExampleGuiLayout>
{
public:
    ExampleGuiLayout() 
    {
        window.closed.connect(this, &ExampleGuiLayout::windowClosed);
               
        window.addWidget(&layout);

        layout.addElement(1);
        layout.addElement(1);
        layout.addElement(1);
        layout.addElement(1);

        EBCpp::EBGuiPadding p;
        p.left = 5;
        p.top = 10;
        p.right = 10;
        p.bottom = 5;
        layout.setPadding(p);

        EBCpp::EBGuiPadding cp;
        cp.left = 5;
        cp.top = 10;
        cp.right = 10;
        cp.bottom = 5;
        layout.setCellPadding(cp);

        layout.addWidget(&button1);
        button1.setText("1");
        button1.setMaxHeight(200);
        button1.clicked.connect(this, &ExampleGuiLayout::buttonClicked);

        layout.addWidget(&button2);
        button2.setText("2");
        button2.setMaxHeight(100);
        button2.clicked.connect(this, &ExampleGuiLayout::buttonClicked);

        layout.addWidget(&label);
        label.setText("???");

        layout.addWidget(&textLine);
        textLine.setText("test");
    }

private:
    EBCpp::EBGuiWindow window;

    EBCpp::EBGuiButton button1;
    EBCpp::EBGuiButton button2;
    EBCpp::EBGuiLabel label;
    EBCpp::EBGuiHorizontalLayout layout;
    EBCpp::EBGuiTextLine textLine;

    EB_SLOT(windowClosed)
    {
        EBCpp::EBEventLoop::getInstance()->exit();
    }

    EB_SLOT(buttonClicked)
    {
        EBCpp::EBObjectPointer<EBCpp::EBGuiButton> button = sender->cast<EBCpp::EBGuiButton>();
        EBCpp::EBString str = EBCpp::EBString("Button Clicked: ") + button->getText();

        label.setText(str);
    }
};

/**
 * @brief Main from the timer example
 *
 * @return int Exit code (0)
 */
int main()
{
    ExampleGuiLayout gui;
    EBCpp::EBEventLoop::getInstance()->exec();
    return 0;
}
