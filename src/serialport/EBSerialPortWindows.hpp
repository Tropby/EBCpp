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

#ifdef __WIN32__

#include "../EBIODevice.hpp"
#include "../profile/EBProfile.hpp"
#include "EBSerialPortBase.hpp"
#include <windows.h>

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
            EB_EXCEPTION("Can not open serial port!");

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
        CloseHandle(osWrite.hEvent);
        bool result = CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
        return result;
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
    virtual int write(const std::string& data)
    {
        return write(data.c_str(), data.size());
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
        EB_PROFILE_FUNC();

        if (thread)
        {
            if (thread->joinable())
            {
                thread->join();
            }
            thread = nullptr;
        }
    }

    void run()
    {
        EB_PROFILE_FUNC();

        char buffer[1];
        bool waitingOnRead;
        DWORD dwRead;

        waitingOnRead = false;
        while (isOpened())
        {
            OVERLAPPED osReader = {0};
            osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            if (osReader.hEvent == NULL)
                EB_EXCEPTION("Can not create osReader.hEvent!");

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

            while (waitingOnRead)
            {
                DWORD dwRes = WaitForSingleObject(osReader.hEvent, 500);
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
            CloseHandle(osReader.hEvent);
        }
    }
};

} // namespace EBCpp

#endif