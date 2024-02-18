/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBList.hpp"
#include "EBObject.hpp"
#include "EBString.hpp"
#include <dirent.h>

namespace EBCpp
{
class EBCrc : public EBObject<EBCrc>
{
public:
    static uint8_t crc8(uint8_t* data, uint16_t length, uint8_t startValue = 0xFF)
    {
        uint8_t bit_counter;
        uint8_t feedback_bit;
        uint16_t i;
        uint8_t crc = startValue;
        uint8_t inByte;

        for (i = 0; i < length; i++)
        {
            bit_counter = 8;
            inByte = *data;
            data++;
            do
            {
                feedback_bit = (crc ^ inByte) & 0x01;
                if (feedback_bit)
                    crc = crc ^ 0x18;
                crc = (crc >> 1) & 0x7F;
                if (feedback_bit)
                    crc = crc | 0x80;
                inByte >>= 1;
                bit_counter--;
            } while (bit_counter > 0);
        }
        return crc;
    }
};
} // namespace EBCpp
