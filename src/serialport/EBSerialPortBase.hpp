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
