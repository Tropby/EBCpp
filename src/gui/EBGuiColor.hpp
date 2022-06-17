#pragma once

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

#include "../EBObject.hpp"

#define EB_COLOR_BLACK EBCpp::EBObjectBase::createObject<EBCpp::EBGuiColor>(0x00, 0x00, 0x00, 0xFF)
#define EB_COLOR_RED EBCpp::EBObjectBase::createObject<EBCpp::EBGuiColor>(0xFF, 0x00, 0x00, 0xFF)
#define EB_COLOR_BLUE EBCpp::EBObjectBase::createObject<EBCpp::EBGuiColor>(0x00, 0xFF, 0x00, 0xFF)
#define EB_COLOR_GREEN EBCpp::EBObjectBase::createObject<EBCpp::EBGuiColor>(0x00, 0x00, 0xFF, 0xFF)
#define EB_COLOR_WHITE EBCpp::EBObjectBase::createObject<EBCpp::EBGuiColor>(0xFF, 0xFF, 0xFF, 0xFF)

namespace EBCpp
{

class EBGuiColor : public EBObject<EBGuiColor>
{
public:
    EBGuiColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF) :
        EBObject(), r(r), g(g), b(b), a(a)
    {
    }

    EBGuiColor(const EBGuiColor& other) : EBObject(), r(other.r), g(other.g), b(other.b), a(other.a)
    {
    }

    void setRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    void setHSL(float h, float s, float l, uint8_t a = 0xFF)
    {
        if (s == 0)
        {
            r = g = b = (unsigned char)(l * 255);
        }
        else
        {
            float v1, v2;
            float hue = (float)h / 360;

            v2 = (l < 0.5) ? (l * (1 + s)) : ((l + s) - (l * s));
            v1 = 2 * l - v2;

            r = (unsigned char)(255 * hueToRGB(v1, v2, hue + (1.0f / 3)));
            g = (unsigned char)(255 * hueToRGB(v1, v2, hue));
            b = (unsigned char)(255 * hueToRGB(v1, v2, hue - (1.0f / 3)));
            this->a = a;
        }
    }

    float getL()
    {
        float r = (this->r / 255.0f);
        float g = (this->g / 255.0f);
        float b = (this->b / 255.0f);

        float mi = min(min(r, g), b);
        float ma = max(max(r, g), b);

        return (ma + mi) / 2;
    }

    float getS()
    {
        float r = (this->r / 255.0f);
        float g = (this->g / 255.0f);
        float b = (this->b / 255.0f);
        float mi = min(min(r, g), b);
        float ma = max(max(r, g), b);

        float delta = ma - mi;
        return (getL() <= 0.5) ? (delta / (ma + mi)) : (delta / (2 - ma - mi));
    }

    float getH()
    {
        float H = 0.0f;
        float r = (this->r / 255.0f);
        float g = (this->g / 255.0f);
        float b = (this->b / 255.0f);
        float mi = min(min(r, g), b);
        float ma = max(max(r, g), b);

        float delta = ma - mi;

        if (delta == 0)
        {
            H = 0;
        }
        else
        {
            float hue;

            if (r == ma)
            {
                hue = ((g - b) / 6) / delta;
            }
            else if (g == ma)
            {
                hue = (1.0f / 3) + ((b - r) / 6) / delta;
            }
            else
            {
                hue = (2.0f / 3) + ((r - g) / 6) / delta;
            }

            if (hue < 0)
                hue += 1;
            if (hue > 1)
                hue -= 1;

            H = (int)(hue * 360);
        }

        return H;
    }

    unsigned char getR()
    {
        return r;
    }

    unsigned char getG()
    {
        return g;
    }

    unsigned char getB()
    {
        return b;
    }

    unsigned char getA()
    {
        return a;
    }

protected:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

private:
    static float min(float a, float b)
    {
        return a <= b ? a : b;
    }

    static float max(float a, float b)
    {
        return a >= b ? a : b;
    }

    float hueToRGB(float v1, float v2, float vH)
    {
        if (vH < 0)
            vH += 1;

        if (vH > 1)
            vH -= 1;

        if ((6 * vH) < 1)
            return (v1 + (v2 - v1) * 6 * vH);

        if ((2 * vH) < 1)
            return v2;

        if ((3 * vH) < 2)
            return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

        return v1;
    }
};

} // namespace EBCpp
