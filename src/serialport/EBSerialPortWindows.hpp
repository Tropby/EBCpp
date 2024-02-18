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

#ifdef __WIN32__

#include "../EBOs.hpp"
#include "../EBIODevice.hpp"
#include "../profile/EBProfile.hpp"
#include "EBSerialPortBase.hpp"

namespace EBCpp
{

/**
 * @brief Interface description of an EBSerialPort device
 *
 */
class EBSerialPort : public EBSerialPortBase
{
public:
    EBSerialPort() : EBSerialPortBase(), handle(INVALID_HANDLE_VALUE)
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
    virtual bool open(DIRECTION direction)
    {
        std::string port = "\\\\.\\COM" + std::to_string(serialPort);
        DWORD access = 0;
        switch (direction)
        {
        case DIRECTION::READ_ONLY:
            access = GENERIC_READ;
            break;
        case DIRECTION::READ_WRITE:
            access = GENERIC_READ | GENERIC_WRITE;
            break;
        case DIRECTION::WRITE_ONLY:
            access = GENERIC_WRITE;
            break;
        default:
            EB_EXCEPTION("Unknown direction for serial port!");
            break;
        }

        handle =
        CreateFileA(port.c_str(), access, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
        if (handle == INVALID_HANDLE_VALUE)
        {
            EB_LOG_ERROR("Can not open serial port!");
            return false;
        }
            
        DCB serialInfo = {0};

        GetCommState(handle, &serialInfo);

        serialInfo.DCBlength = sizeof(DCB);
        serialInfo.BaudRate = baudrate;
        serialInfo.fBinary = TRUE;
        serialInfo.fParity = TRUE;
        serialInfo.fErrorChar = FALSE;
        serialInfo.fNull = FALSE;
        serialInfo.fAbortOnError = FALSE; // TODO
        serialInfo.ByteSize = byteSize;

        switch (parity)
        {
        case PARITY::P_NO:
            serialInfo.Parity = NOPARITY;
            break;
        case P_ODD:
            serialInfo.Parity = ODDPARITY;
            break;
        case P_EVEN:
            serialInfo.Parity = EVENPARITY;
            break;
        default:
            EB_EXCEPTION("Unknown parity setting!");
            break;
        }

        switch (stopBits)
        {
        case SB_1:
            serialInfo.StopBits = ONESTOPBIT;
            break;
        case SB_3_2:
            serialInfo.StopBits = ONE5STOPBITS;
            break;
        case SB_2:
            serialInfo.StopBits = TWOSTOPBITS;
            break;
        default:
            EB_EXCEPTION("Unknown stop bits setting!");
            break;
        }

        serialInfo.ErrorChar = 0xFF;

        SetCommState(handle, &serialInfo);

        // Create this writes OVERLAPPED structure hEvent.
        osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (osWrite.hEvent == NULL)
            return false;

        startThread();
        return true;
    }

    /**
     * @brief Returns the state of the i/o device
     *
     * @return true if the i/o device is opened
     * @return false if the i/o device is not open
     */
    virtual bool isOpened()
    {
        return handle != INVALID_HANDLE_VALUE;
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
        if( handle == INVALID_HANDLE_VALUE )
            return false;

        try
        {
            HANDLE t = handle;
            handle = INVALID_HANDLE_VALUE;

            CloseHandle(osReader.hEvent);
            CloseHandle(osWrite.hEvent);
            bool result = CloseHandle(t);

            joinThread();

            return result;
        }
        catch(...)
        {
            return false;
        }
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
        if (!isOpened())
            return -1;

        DWORD dwWritten;
        int fRes;

        // Issue write.
        if (!WriteFile(handle, data, length, &dwWritten, &osWrite))
        {
            if (GetLastError() != ERROR_IO_PENDING)
            {
                fRes = -1;
            }
            else
            {
                // Write is pending.
                if (!GetOverlappedResult(handle, &osWrite, &dwWritten, TRUE))
                    fRes = -1;
                else
                    // Write operation completed successfully.
                    fRes = dwWritten;
            }
        }
        else
            // WriteFile completed immediately.
            fRes = dwWritten;

        ResetEvent(osWrite.hEvent);
        return fRes;
    }

    /**
     * @brief Writes a string to the output device
     *
     * @param data String to wirte to the output device
     * @return int Bytes written to the output device
     */
    virtual int write(const EBString& data)
    {
        return write(data.dataPtr(), data.length());
    }

private:
    HANDLE handle;
    OVERLAPPED osWrite;

    void startThread()
    {
        EB_PROFILE_FUNC();
        thread = std::unique_ptr<std::thread>(new std::thread(std::bind(&EBSerialPort::run, this)));
    }

    /**
     * @brief Waiting for the Thread to join it with the current thread
     *
     */
    void joinThread()
    {
        if (thread)
        {
            if (thread->joinable())
            {
                thread->join();
            }
            thread = nullptr;
        }
    }

    OVERLAPPED osReader = {0};
    void run()
    {
        EB_PROFILE_FUNC();

        char buffer[1];
        bool waitingOnRead;
        DWORD dwRead;

        waitingOnRead = false;

        osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (osReader.hEvent == NULL)
            EB_EXCEPTION("Can not create osReader.hEvent!");

        while (isOpened())
        {
            if (!waitingOnRead)
            {
                // Read more than 1 byte
                if (!ReadFile(handle, buffer, 1, &dwRead, &osReader))
                {
                    if (GetLastError() != ERROR_IO_PENDING)
                        EB_EXCEPTION("Can not read from serial port!");
                    else
                        waitingOnRead = true;
                }
                else
                {
                    if (dwRead > 0)
                    {
                        const std::lock_guard<std::mutex> lock(mutex);

                        bool emit = data.size() == 0;
                        for (int i = 0; i < dwRead; i++)
                            data.push_back(buffer[i]);
                        if (emit)
                            EB_EMIT(readReady);
                    }
                }
            }

            while (waitingOnRead && isOpened())
            {
                DWORD dwRes = WaitForSingleObject(osReader.hEvent, 500);

                if(!isOpened())
                    break;

                switch (dwRes)
                {
                // Read completed.
                case WAIT_OBJECT_0:
                    if (!GetOverlappedResult(handle, &osReader, &dwRead, FALSE))
                    {
                        EB_EXCEPTION("ERROR IN SERIAL PORT COMMUNICATION2");
                    }
                    else
                    {
                        const std::lock_guard<std::mutex> lock(mutex);
                        bool emit = data.size() == 0;
                        for (int i = 0; i < dwRead; i++)
                            data.push_back(buffer[i]);
                        if (emit)
                            EB_EMIT(readReady);
                    }
                    waitingOnRead = false;

                    break;
                case WAIT_TIMEOUT:
                    break;
                }
            }            
        }
    }
};

} // namespace EBCpp

#endif