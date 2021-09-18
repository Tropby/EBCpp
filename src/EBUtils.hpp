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

    /**
     * @brief returns the lower case of an string
     * 
     * @param s Input string
     * @return std::string The input string in lower case
     */
    static std::string toLower( std::string s )
    {
        std::transform(s.begin(), s.end(), s.begin(), 
            [](unsigned char c){ return std::tolower(c); }
        );
        return s;
    }

    /**
     * @brief Decodes an URL.
     * 
     * @param s Encoded URL
     * @return std::string Decodes URL
     */
    static std::string urlDecode( std::string s )
    {
        std::string ret;
        char ch;

        for( int i = 0; i < s.length(); i++ )
        {
            if( s[i] == '%' ) 
            {
                int ii;
                sscanf( s.substr(i+1,2).c_str(), "%x", &ii );
                ch=static_cast<char>(ii);
                ret += ch;
                i = i + 2;
            }
            else if( s[i] == '+' )
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
        static std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }

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
};

} // namespace EBCpp
