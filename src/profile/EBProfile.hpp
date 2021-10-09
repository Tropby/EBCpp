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

#include "../EBObject.hpp"
#include "EBLogger.hpp"
#include <chrono>

#ifdef EB_PROFILE_CONSOLE
#define EB_PROFILE
#endif

#ifdef EB_PROFILE
#define EB_PROFILE_FUNC() EBCpp::EBProfileCall _ebprofilecall(__FILE__, __LINE__, __func__, this->watchCount());
#define EB_PROFILE_CLASS()
#else
#define EB_PROFILE_FUNC()
#define EB_PROFILE_CLASS()
#endif

namespace EBCpp
{

class EBProfileCall;
typedef EBObjectPointer<EBProfileCall> EBProfileCallPtr;

/**
 * @brief Class EBProfile proviedes the funktionality to log and
 *        proifile the behavior of an software written with EBCpp
 *
 */
class EBProfile : public EBObject<EBProfile>
{
public:
    /**
     * @brief Get the Singelton object
     *
     * @return EBObjectPointer<EBProfile> Singelton object
     */
    static EBObjectPointer<EBProfile> getInstance()
    {
        /// TODO: Add instance for each Thread.
        if (instance == nullptr)
        {
            instance = EBObjectPointer<EBProfile>(new EBProfile());
        }
        return instance;
    }

    void addToStack(EBProfileCallPtr profileCall)
    {
        callStack.push_back(profileCall);
    }

    void removeFromStack(EBProfileCallPtr profileCall)
    {
        callStack.pop_back();
    }

private:
    /**
     * @brief Construct a new EBProfile object
     *
     */
    EBProfile()
    {
    }

    //! Singelton object of the EBProfile class
    static inline EBObjectPointer<EBProfile> instance = EBObjectPointer<EBProfile>(nullptr);

    std::list<EBProfileCallPtr> callStack;
};

/**
 * @brief Class thats provides informations of an EBProfileCall
 *
 */
class EBProfileCall : public EBObject<EBProfileCall>
{
public:
    /**
     * @brief Construct a new EBProfileCall object
     *
     * @param file __FILE__ Current file
     * @param line __LINE__ Current line
     * @param method  __FUNCTION__ Name of the current function
     * @param sharedPointer Shared pointer count that are pointing to the current object
     */
    EBProfileCall(std::string file, int line, std::string method, int sharedPointer) :
        file(file), line(line), method(method), sharedPointer(sharedPointer)
    {
        start = std::chrono::high_resolution_clock::now();

        EBProfile::getInstance()->addToStack(this);

        EB_LOG_PROFILE("START [Obj: " << EBObjectBase::_counter << " ObjP: " << EBObjectPointerBase::_counter
                                      << " SPtr: " << sharedPointer << "] >> " << file << ":" << line << " @ "
                                      << method);
    }

    /**
     * @brief Destroy the EBProfileCall object
     *
     */
    ~EBProfileCall()
    {
        auto stop = std::chrono::high_resolution_clock::now();
        auto int_s = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        EBProfile::getInstance()->removeFromStack(this);

        EB_LOG_PROFILE("STOP [Obj: " << EBObjectBase::_counter << " ObjP: " << EBObjectPointerBase::_counter
                                     << " SPtr: " << sharedPointer << "] >> " << file << ":" << line << " @ " << method
                                     << " (T=" << int_s.count() << " us)");
    }

private:
    //! File that has created the profile call
    std::string file;

    //! Method of the profile call
    std::string method;

    //! Line of the profile call
    int line;

    //! SharedPointer count of the calling obejct
    int sharedPointer;

    //! Time of the profile call
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

} // namespace EBCpp
