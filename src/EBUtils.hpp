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

#include <unistd.h>
#include <pthread.h>

#include "EBString.hpp"
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
     * @return EBString ip address of the hostname
     */
    static EBString hostnameToIp(const EBString& hostname)
    {
        struct addrinfo hints, *servinfo, *p;
        struct sockaddr_in* h;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(hostname.dataPtr(), "http", &hints, &servinfo)) != 0)
        {
            return "";
        }

        // loop through all the results and ste the ip to the last result
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
     * @brief Converts an IPv4 address in its string representation to an unsinged int 32 bit
     * 
     * @param ip The string representation
     * @return uint32_t The ip as unsinged int 32 bit 
     */
    static uint32_t ipToInt(EBString& ip)
    {
        EBList<EBString> list = ip.split(".");

        if( list.getSize() != 4)
        {
            throw new EBException("Can not convert wrong formated ipv4 addresses!");
        }

        uint32_t result = 0;

        result += ((uint8_t)list.get(0).toInt64()) << 24;
        result += ((uint8_t)list.get(1).toInt64()) << 16;
        result += ((uint8_t)list.get(2).toInt64()) << 8;
        result += ((uint8_t)list.get(3).toInt64()) << 0;

        return result;
    }

    /**
     * @brief Converts an integer to its string representation
     * 
     * @param ip IP as an unsinged int 32 bit
     * @return EBString The string represenatation of an IPv4 address
     */
    static EBString intToIp(uint32_t ip)
    {
        return intToStr((uint32_t)((uint8_t)(ip >> 24))) + "." + intToStr((uint32_t)((uint8_t)(ip >> 16))) + "." +
               intToStr((uint32_t)((uint8_t)(ip >> 8))) + "." + intToStr((uint32_t)((uint8_t)(ip >> 0)));
    }

    /**
     * @brief Sets the thread name
     *
     * @param threadName Name of the current thread
     */
    static void setThreadName(std::string threadName)
    {
        #ifdef PTHREAD_SETNAME
            pthread_setname_np(pthread_self(), threadName.c_str());
        #endif
    }

    /**
     * @brief Get the Thread Name object
     *
     * @return std::string Name of the current thread
     */
    static std::string getThreadName()
    {
        #ifdef PTHREAD_SETNAME
            char buffer[128];
            pthread_getname_np(pthread_self(), buffer, sizeof buffer);
            return buffer;
        #else
            return "";
        #endif
    }

    static std::string binToHex(char* data, uint32_t len)
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

    template <typename T>
    static EBString intToStr(T i)
    {
        std::stringstream stream;
        stream << std::dec << i;
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
    static std::string intToHex(T i, bool prefix = false)
    {
        std::stringstream stream;
        stream << ( prefix ? "0x" : "" ) << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;
        return stream.str();
    }

    static std::string intToHex(int i, int size, bool prefix = false)
    {
        std::stringstream stream;
        stream << ( prefix ? "0x" : "" ) << std::setfill('0') << std::setw(size) << std::hex << i;
        return stream.str();
    }    

    static std::string currentWorkingDirectory()
    {
        char buffer[1024];
        getcwd(buffer, 1024);
        return buffer;
    }

    static std::string charToHex(uint8_t i, bool prefix = true)
    {
        std::stringstream stream;
        stream << (prefix?"0x":"") << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)i;
        return stream.str();
    }

    /**
     * @brief returns the lower case of an string
     *
     * @deprecated std::string should not be used within EBCpp anymore. Use EBString
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
     * @deprecated std::string should not be used within EBCpp anymore. Use EBString
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

    static EBString currentDateTimeString()
    {
        return currentDateString() + "T" + currentTimeString();
    }

    static EBString currentDateString()
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

    static EBString doubleToSizeWithUnit(double input)
    {
        EBString unit[] = {"B", "kB", "MB", "GB", "TB"};
        int index = 0;

        while( input > 1024 )
        {
            input /= 1024;
            index++;
        }
        
        char c[64];
        sprintf(c, "%.1f %s", input, unit[index].dataPtr());
        return EBString(c, strlen(c));
    }

    /**
     * @brief Converts a double to the corresponding string representation
     * 
     * @param input The double to convert
     * @return EBString The converted string
     */
    static EBString doubleToStr(double input)
    {
        char c[64];
        sprintf(c, "%g", input);
        return EBString(c, strlen(c));
    }

    /**
     * @brief Returns the current unixtime 
     * 
     * @return int64_t The current unixtime
     */
    static int64_t unixtime()
    {
        const auto p1 = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(
                   p1.time_since_epoch()).count();
    }
    
    /**
     * @brief Returns the current time as a string in the format HH:MM:SS
     * 
     * @return EBString The formated string of the current time
     */
    static EBString currentTimeString()
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

    /**
     * @brief Replaces a string in a string
     * 
     * @deprecated std::string should not be used within EBCpp
     * @param subject 
     * @param search 
     * @param replace 
     * @return std::string 
     */
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

    /**
     * @brief This will startup the TCP functionality of the system (used for WSA on windows)
     * 
     */
    static void startupTCP()
    {
        static bool inited = false;
        if (!inited)
        {
#ifdef __WIN32__
            WORD versionWanted = MAKEWORD(1, 1);
            WSADATA wsaData;
            WSAStartup(versionWanted, &wsaData);
            inited = true;
#endif
        }
    }

    /**
     * @brief Get the Hostname of the system
     * 
     * @return EBString Hostname of the system
     */
    static EBString getHostname()
    {
        char src_name[256];
        if( gethostname(src_name, sizeof(src_name)) >= 0 )
        {
            return src_name;
        }
        return EBString();
    }
};

} // namespace EBCpp
