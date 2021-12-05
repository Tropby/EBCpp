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

#include "EBObject.hpp"

#include <string.h>
#include <string>

namespace EBCpp
{

class EBString : public EBObject<EBString>
{
public:
    EBString()
    {
        this->size = 0;
        this->data = new char[1];
        this->data[0] = 0x00;
    }

    EBString(const char* data, uint32_t size)
    {
        this->size = size;
        this->data = new char[this->size + 1];
        memcpy(this->data, data, this->size + 1);
        this->data[size] = 0x00;
    }

    EBString(const char* data)
    {
        this->size = strlen(data);
        this->data = new char[this->size + 1];
        memcpy(this->data, data, this->size + 1);
    }

    EBString(const EBString& other)
    {
        this->size = other.size;
        this->data = new char[this->size + 1];
        memcpy(this->data, other.data, this->size + 1);
    }

    EBString(const std::string data)
    {
        this->size = data.size();
        this->data = new char[this->size + 1];
        memcpy(this->data, data.c_str(), this->size + 1);
    }

    ~EBString()
    {
        delete[] this->data;
    }

    const char* dataPtr() const
    {
        return this->data;
    }

    bool empty() const
    {
        return length() == 0;
    }

    unsigned int length() const
    {
        return this->size;
    }

    int compare(const EBString& other) const
    {
        return strcmp(this->data, other.data);
    }

    bool equals(const EBString& other) const
    {
        return compare(other) == 0;
    }

    bool contains(const EBString& other) const
    {
        return strstr(this->data, other.data) != NULL;
    }

    bool startsWith(const EBString& other) const
    {
        EBString substr = this->mid(0, other.length());
        return substr == other;
    }

    EBString mid(uint64_t start, uint64_t length = 0) const
    {
        if (start >= size)
        {
            return "";
        }

        if (length == 0)
        {
            length = (size - start);
        }

        if (size < start + length)
            length = (size - start);

        char newString[length + 1];
        memcpy(newString, data + start, length);
        newString[length] = 0x00;

        return newString;
    }

    int toInt(uint8_t base = 10)
    {
        return std::strtol(data, NULL, base);
    }

    std::string toStdString() const
    {
        return std::string(data, size);
    }

    int32_t indexOf(const EBString& other) const
    {
        char* pos = strstr(this->data, other.data);
        if (pos == NULL)
        {
            return -1;
        }

        return pos - this->data;
    }

    const EBString trim() const
    {
        int start = 0;
        while( isspace(data[start++]) )
            ;

        int len = length() - 1;
        while (isspace(data[len--]))
            ;

        return mid(start - 1, len - start + 3);
    }

    bool operator==(const EBString& other) const
    {
        return compare(other) == 0;
    }

    bool operator!=(const EBString& other) const
    {
        return compare(other) != 0;
    }

    EBString& operator=(const EBString& other)
    {
        char newData[other.size + 1];
        delete[] data;
        data = new char[other.size];
        this->size = other.size;
        memcpy(data, other.data, other.size + 1);
        return *this;
    }

    bool operator<(const EBString& other) const
    {
        return compare(other) < 0;
    }

    bool operator>(const EBString& other) const
    {
        return compare(other) > 0;
    }

    EBString operator+(const EBString& other) const
    {
        char newData[this->size + other.size + 1];
        memcpy(newData, this->data, this->size);
        memcpy(newData + this->size, other.data, other.size + 1);
        return newData;
    }

    EBString& operator+=(const EBString& other)
    {
        char* newData = new char[this->size + other.size + 1];
        memcpy(newData, this->data, this->size);
        delete[] this->data;
        memcpy(newData + this->size, other.data, other.size + 1);
        this->data = newData;
        this->size = this->size + other.size;

        return *this;
    }

private:
    char* data;
    uint64_t size;
};

} // namespace EBCpp

inline std::ostream& operator<<(std::ostream& os, const EBCpp::EBString& str)
{
    return os << str.dataPtr();
}
