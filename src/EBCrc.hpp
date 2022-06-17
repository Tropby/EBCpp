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
