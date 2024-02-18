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

#include "EBObject.hpp"
#include "EBString.hpp"

namespace EBCpp
{

/**
 * @brief Interface description of an input/output device
 *
 */
template <class T>
class EBIODevice : public EBObject<T>
{
public:
    /**
     * @brief Construct a new EBIODevice object
     *
     * @param parent The parent of the IODevice
     */
    EBIODevice()
    {
    }

    /**
     * @brief Direction of the device
     */
    typedef enum
    {
        READ_ONLY,
        WRITE_ONLY,
        READ_WRITE
    } DIRECTION;

    /**
     * @brief Opens an input / output device
     *
     * @param direction The direction to open the device
     * @return true if the device could be opened
     * @return false if the device couldn't be opened
     * @throws May throws an exception
     */
    virtual bool open(DIRECTION direction) = 0;

    /**
     * @brief Returns the state of the i/o device
     *
     * @return true if the i/o device is opened
     * @return false if the i/o device is not open
     */
    virtual bool isOpened() = 0;

    /**
     * @brief Close the i/o device
     *
     * @return true if the i/o device could be closed
     * @return false on any error
     * @throws May throws an exception
     */
    virtual bool close() = 0;

    /**
     * @brief Writes data to the output device
     *
     * @param data Pointer to the data to write
     * @param length Size of the data in bytes
     * @return int Bytes written to the output device
     */
    virtual int write(const char* data, int length) = 0;

    /**
     * @brief Writes a string to the output device
     *
     * @param data String to wirte to the output device
     * @return int Bytes written to the output device
     */
    virtual int write(const EBString& data) = 0;

    /**
     * @brief Read data from the socket
     *
     * @param data Buffer for the read data
     * @param maxLength Size of the buffer
     * @return int Bytes read
     */
    virtual int read(char* data, int maxLength) = 0;    

    /**
     * @brief Checks if the data buffer contains "\\n"
     *
     * @return true if the data buffer contains "\\n"
     * @return false if the data buffer does not contain "\\n"
     */
    virtual bool canReadLine() = 0;

    /**
     * @brief Read a line from the input device
     *
     * @return EBString line data
     * @throws May throws an error if no line is available
     */
    virtual const EBString readLine() = 0;

    /**
     * @brief Returns true if the end of stream is reached
     * 
     * @return true if at the end of the io stream
     * @return false otherwise
     */
    virtual bool atEnd() = 0;

    /**
     * @brief Set the filename
     *
     * @param fileName device to open (eg. /home/user/test.txt, tcp://127.0.0.1:8080, ...)
     */
    void setFileName(const EBString& fileName)
    {
        this->fileName = fileName;
    }

    /**
     * @brief Get the filename
     *
     * @return EBString filename of the device
     */
    const EBString& getFileName()
    {
        return fileName;
    }

private:
    EBString fileName;
};

} // namespace EBCpp
