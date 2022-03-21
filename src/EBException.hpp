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

#include <exception>
#include <string>

namespace EBCpp
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define EB_AT __FILE__ ":" TOSTRING(__LINE__) + std::string(" [") + std::string(__FUNCTION__) + std::string("]")

//! Macro for exception with file and line
#define EB_EXCEPTION(message) throw EBCpp::EBException(std::string(message) + "\n\t in " EB_AT)

/**
 * @brief Exceptions from EBCpp
 *
 */
class EBException : public std::exception
{
public:
    /**
     * @brief Construct a new EBException object
     *
     * @param message Message for the exception
     */
    EBException(std::string message) : std::exception(), message(message)
    {
    }

    /**
     * @brief Returns the message as String
     *
     * @return std::string Exeption message
     */
    std::string what()
    {
        return message;
    }

    /**
     * @brief Returns the message as String
     *
     * @return const char* Exeption message
     */
    virtual const char* what() const 
    #ifdef _GLIBCXX_TXN_SAFE_DYN
        _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
    #endif
    #ifdef _NOEXCEPT
        _NOEXCEPT
    #endif
    {
        return message.c_str();
    }

private:
    std::string message;
};

} // namespace EBCpp