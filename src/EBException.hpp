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

#include <exception>
#include <string>

namespace EBCpp
{

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define EB_AT __FILE__ ":" TOSTRING(__LINE__) + std::string(" [") + std::string(__FUNCTION__) + std::string("]")

//! Macro for exception with file and line
#define EB_EXCEPTION(message) throw EBCpp::EBException(std::string(message) + "\n\t in " EB_AT)

#define EB_EXCEPTION_EX(type, message) throw EBCpp::type(message + "\n\t in " EB_AT)

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
    explicit EBException(std::string message) : std::exception(), message(message)
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
     * @overide
     * @return const char* Exeption message
     */
    const char* what() const
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
    const std::string message;
};

} // namespace EBCpp