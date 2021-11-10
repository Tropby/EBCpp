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
#include "../src/gui/EBGuiPicture.hpp"
#include "../src/gui/EBGuiWidget.hpp"
#include "../src/gui/EBGuiWindow.hpp"

#include <iostream>

class ExampleGuiImage : public EBCpp::EBObject<ExampleGuiImage>
{
public:
    ExampleGuiImage()
    {
        window.closed.connect(this, &ExampleGuiImage::windowClosed);
        window.setTitle("GUI Image Test");
        window.addWidget(&widget);

        widget.setX(20);
        widget.setY(20);
        widget.setWidth(300);
        widget.setHeight(300);

        widget.addWidget(&imageBMP);
        imageBMP.setX(20);
        imageBMP.setY(20);
        imageBMP.setWidth(200);
        imageBMP.setHeight(200);
        imageBMP.setImage("../examples/testImage.BMP");

        widget.addWidget(&imageJPG);
        imageJPG.setX(340);
        imageJPG.setY(20);
        imageJPG.setWidth(200);
        imageJPG.setHeight(300);
        imageJPG.setImage("../examples/testImage.JPG");

        widget.addWidget(&imagePNG);
        imagePNG.setX(20);
        imagePNG.setY(240);
        imagePNG.setWidth(300);
        imagePNG.setHeight(200);
        imagePNG.setImage("../examples/testImage.PNG");
    }

    ~ExampleGuiImage()
    {        
    }

private:
    EBCpp::EBGuiWindow window;
    EBCpp::EBGuiWidget widget;
    EBCpp::EBGuiPicture imageBMP;
    EBCpp::EBGuiPicture imageJPG;
    EBCpp::EBGuiPicture imagePNG;

    EB_SLOT(windowClosed)
    {
        EBCpp::EBEventLoop::getInstance()->exit();
    }
};

/**
 * @brief Main from the timer example
 *
 * @return int Exit code (0)
 */
int main()
{    
    ExampleGuiImage gui;
    EBCpp::EBEventLoop::getInstance()->exec();
    return 0;
}