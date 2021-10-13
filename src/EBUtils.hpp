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

#include <iomanip>
#include <sstream>
#include <string>

#include "socket/tcp/EBTcpHeader.hpp"

namespace EBCpp
{

/**
 * @brief Class that provides utility functions
 *
 */
class EBUtils
{
public:
    /**
     * @brief converts a hostname to its corrosponding ip address
     *
     * @param hostname Hostname that will be converted
     * @return std::string ip address of the hostname
     */
    static std::string hostnameToIp(std::string hostname)
    {
        struct addrinfo hints, *servinfo, *p;
        struct sockaddr_in* h;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(hostname.c_str(), "http", &hints, &servinfo)) != 0)
        {
            return "";
        }

        // loop through all the results and connect to the first we can
        char ip[64];
        for (p = servinfo; p != NULL; p = p->ai_next)
        {
            h = (struct sockaddr_in*)p->ai_addr;
            strcpy(ip, inet_ntoa(h->sin_addr));
        }

        freeaddrinfo(servinfo); // all done with this structure
        return ip;
    }

    /**
     * @brief Sets the thread name
     *
     * @param threadName Name of the current thread
     */
    static void setThreadName(std::string threadName)
    {
        pthread_setname_np(pthread_self(), threadName.c_str());
    }

    /**
     * @brief Get the Thread Name object
     * 
     * @return std::string Name of the current thread
     */
    static std::string getThreadName()
    {
        char buffer[128];
        pthread_getname_np(pthread_self(), buffer, sizeof buffer);
        return buffer;
    }

    static std::string binToHex(char * data, uint32_t len)
    {
        std::stringstream stream;
        while (len > 0)
        {
            stream << std::setfill('0') << std::setw(2) << std::hex << (int)(uint8_t)*data;
            data++;
            len--;
        }
        return stream.str();
    }

    /**
     * @brief converts an integer/long to its hex representation
     *
     * @tparam T Type of the parameter i
     * @param i integer to convert to hex
     * @return std::string hex string of the parameter i
     */
    template <typename T>
    static std::string intToHex(T i)
    {
        std::stringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;
        return stream.str();
    }

    static std::string currentWorkingDirectory()
    {
        char buffer[1024];
        getcwd(buffer, 1024);
        return buffer;
    }

    static std::string charToHex(char i)
    {
        std::stringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(2) << std::hex << (int)i;
        return stream.str();
    }

    /**
     * @brief returns the lower case of an string
     *
     * @param s Input string
     * @return std::string The input string in lower case
     */
    static std::string toLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    /**
     * @brief Decodes an URL.
     *
     * @param s Encoded URL
     * @return std::string Decodes URL
     */
    static std::string urlDecode(std::string s)
    {
        std::string ret;
        char ch;

        for (int i = 0; i < s.length(); i++)
        {
            if (s[i] == '%')
            {
                int ii;
                sscanf(s.substr(i + 1, 2).c_str(), "%x", &ii);
                ch = static_cast<char>(ii);
                ret += ch;
                i = i + 2;
            }
            else if (s[i] == '+')
            {
                ret += " ";
            }
            else
            {
                ret += s[i];
            }
        }
        return ret;
    }

    /**
     * @brief Returns the uptime of the application in milliseconds.
     *
     * @return int64_t Milliseconds since the software was started.
     */
    static int64_t uptime()
    {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - programmStartTime).count();
    }
    static inline std::chrono::steady_clock::time_point programmStartTime = std::chrono::steady_clock::now();

    /**
     * @brief Trims a string and returns the trimmed string
     *
     * @param s String that should be trimmed
     * @return std::string trimmed string
     */
    static std::string trim(const std::string& s)
    {
        auto isWhitespace = [](const char c) { return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == 11; };

        int left = 0, right = s.length() - 1;

        while (left <= right && isWhitespace(s[left]))
            ++left;

        while (right > left && isWhitespace(s[right]))
            --right;

        return s.substr(left, 1 + right - left);
    }

    static std::string currentDateTimeString()
    {
        return currentDateString() + "T" + currentTimeString();
    }

    static std::string currentDateString()
    {
        auto now = std::chrono::system_clock::now();
        // convert to std::time_t in order to convert to std::tm (broken time)
        auto timer = std::chrono::system_clock::to_time_t(now);

        // convert to broken time
        std::tm bt = *std::localtime(&timer);

        std::ostringstream oss;

        oss << std::put_time(&bt, "%Y-%m-%d"); // YYYY-MM-DD

        return oss.str();
    }

    static std::string currentTimeString()
    { 
        auto now = std::chrono::system_clock::now();

        // get number of milliseconds for the current second
        // (remainder after division into seconds)
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // convert to std::time_t in order to convert to std::tm (broken time)
        auto timer = std::chrono::system_clock::to_time_t(now);

        // convert to broken time
        std::tm bt = *std::localtime(&timer);

        std::ostringstream oss;

        oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

        return oss.str();
    }

    static std::string replaceString(std::string subject, std::string search, std::string replace)
    {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos)
        {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    static unsigned char calCRC8(unsigned char* data, unsigned char length)
    {
        unsigned char bit_counter;
        unsigned char feedback_bit;
        unsigned char i;
        unsigned char crc = 0xFF;
        unsigned char inByte;

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
