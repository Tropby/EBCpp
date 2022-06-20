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

#include <fstream>
#include <sys/stat.h>

#include "EBIODevice.hpp"
#include "EBString.hpp"

namespace EBCpp
{

/**
 * @brief Interface description of an input/output device
 *
 */
class EBFile : public EBIODevice < EBFile>
{
public:
    /**
     * @brief Construct a new EBIODevice object
     *
     * @param parent The parent of the IODevice
     */
    EBFile() 
    {
    }

    /**
     * @brief Opens an input / output device
     *
     * @param direction The direction to open the device
     * @return true if the device could be opened
     * @return false if the device couldn't be opened
     * @throws May throws an exception
     */
    virtual bool open(EBIODevice::DIRECTION direction)
    {
        std::ios_base::openmode om;
        switch(direction)
        {
            case READ_WRITE:
                om = std::ios::in | std::ios::out;
                break;

            case READ_ONLY:
                om = std::ios::in;
                break;

            case WRITE_ONLY:
                om = std::ios::out;
                break;
        }
        
        file.open(getFileName().toStdString(), om | std::ios::binary);
        return file.is_open();
    }

    /**
     * @brief Returns the state of the i/o device
     *
     * @return true if the i/o device is opened
     * @return false if the i/o device is not open
     */
    virtual bool isOpened()
    {
        return file.is_open();
    }

    /**
     * @brief Close the i/o device
     *
     * @return true if the i/o device could be closed
     * @return false on any error
     * @throws May throws an exception
     */
    virtual bool close()
    {
        file.close();
        return false;
    }

    /**
     * @brief Writes data to the output device
     *
     * @param data Pointer to the data to write
     * @param length Size of the data in bytes
     * @return int Bytes written to the output device
     */
    virtual int write(const char* data, int length)
    {
        file.write( data, length );
        return length;
    }

    /**
     * @brief Writes a string to the output device
     *
     * @param data String to wirte to the output device
     * @return int Bytes written to the output device
     */
    virtual int write(const std::string& data)
    {
        file << data;
        return data.size();
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
        file.read(data, maxLength);
        return file.gcount();
    }

    /**
     * @brief Checks if the file contains another line
     *
     * @return true if the file contains "\\n"
     * @return false if the file does not contain "\\n"
     */
    virtual bool canReadLine()
    {
        std::string str;

        std::streampos p = file.tellg();
        bool result = false;
        if( std::getline(file, str) )
            result = true;
        file.seekg(p);

        return result;
    }

    /**
     * @brief Read a line from the input device
     *
     * @return std::string line data
     * @throws May throws an error if no line is available
     */
    virtual const EBString readLine()
    {
        std::string str;
        std::getline(file, str);
        return EBString(str);
    }

    /**
     * @brief Reads the whole file as string
     * 
     * @return std::string the file content
     */
    virtual const EBString readAll()
    {
        EBString result;

        char c[1024];
        while( int n = read(c, 1024) )
        {
            result += EBString(c, n);
        }
        return result;
    }

    /**
     * @brief Returns true if the end of stream is reached
     *
     * @return true if at the end of the io stream
     * @return false otherwise
     */
    virtual bool atEnd()
    {
        return file.eof();
    }

    bool exists()
    {        
        return exists(getFileName());
    }

    static int64_t getFileSize(EBString file)
    {
        FILE * fp = fopen(file.dataPtr(), "r");
        fseek(fp, 0, SEEK_END); // seek to end of file
        return ftell(fp); // get current file pointer
    }

    static inline bool exists( const EBString& filename )
    {
        struct stat buffer;
        return (stat(filename.dataPtr(), &buffer) == 0);
    }

    static bool remove(EBString filename)
    {
        return std::remove(filename.dataPtr()) == 0;
    }

private:
    std::fstream file;
};

} // namespace EBCpp
