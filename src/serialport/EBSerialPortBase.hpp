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

#include "../EBEvent.hpp"

namespace EBCpp
{
class EBSerialPortBase : public EBIODevice<EBSerialPortBase>
{
public:
    EBSerialPortBase() : serialPort(0), baudrate(9600), byteSize(8), parity(P_NO), stopBits(SB_1)
    {
    }

    enum STOP_BITS
    {
        SB_1,
        SB_3_2,
        SB_2
    };

    enum PARITY
    {
        P_NO,
        P_EVEN,
        P_ODD
    };

    virtual void setPort(int serialPort)
    {
        this->serialPort = serialPort;
    }

    virtual int getPort()
    {
        return serialPort;
    }

    virtual void setBaudrate(uint32_t baudrate)
    {
        this->baudrate = baudrate;
    }

    virtual uint32_t getBaudrate()
    {
        return baudrate;
    }

    virtual void setByteSize(uint8_t byteSize)
    {
        this->byteSize = byteSize;
    }

    virtual uint8_t getByteSize()
    {
        return byteSize;
    }

    virtual void setParity(PARITY parity)
    {
        this->parity = parity;
    }

    virtual PARITY getParity()
    {
        return parity;
    }

    virtual void setStopBits(STOP_BITS stopBits)
    {
        this->stopBits = stopBits;
    }

    virtual STOP_BITS getStopBits()
    {
        return stopBits;
    }

    /**
     * @brief Read data from the socket
     *
     * @param data Buffer for the read data
     * @param maxLength Size of the buffer
     * @return int Bytes read
     */
    virtual int read(char* data, int maxLength)
    {
        const std::lock_guard<std::mutex> lock(mutex);

        int test = this->data.size();
        int size = this->data.size() < maxLength ? this->data.size() : maxLength;

        for (int i = 0; i < size; i++)
        {
            *data = this->data.front();
            data++;
            this->data.pop_front();
        }

        return size;
    }

    /**
     * @brief Checks if the data buffer contains "\\n"
     *
     * @return true if the data buffer contains "\\n"
     * @return false if the data buffer does not contain "\\n"
     */
    virtual bool canReadLine()
    {
        EB_PROFILE_FUNC();

        const std::lock_guard<std::mutex> lock(mutex);
        return (std::find(data.begin(), data.end(), '\n') != data.end());
    }

    /**
     * @brief Read a line from the input device
     *
     * @return std::string line data
     * @throws May throws an error if no line is available
     */
    virtual const EBString readLine()
    {
        EB_PROFILE_FUNC();

        const std::lock_guard<std::mutex> lock(mutex);

        bool found = (std::find(data.begin(), data.end(), '\n') != data.end());
        if (found)
        {
            std::string result;
            char c = 0;
            while (c != '\n')
            {
                c = this->data.front();
                result += c;
                this->data.pop_front();
            }
            return result;
        }
        else
        {
            EB_EXCEPTION("Can not read line. No \\n found in received data!");
        }

        return std::string();
    }

    /**
     * @brief Returns true if the end of stream is reached
     *
     * @return true if at the end of the io stream
     * @return false otherwise
     */
    virtual bool atEnd()
    {
        return isOpened();
    }

    EB_SIGNAL(readReady);

protected:
    STOP_BITS stopBits;
    PARITY parity;
    std::unique_ptr<std::thread> thread;
    int serialPort;
    uint32_t baudrate;
    uint8_t byteSize;
    std::list<char> data;
    std::mutex mutex;
};

} // namespace EBCpp
