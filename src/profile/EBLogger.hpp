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
#include <mutex>
#include <sstream>

#define EB_LOG(msg)                                                                                                    \
    {                                                                                                                  \
        std::stringstream _ss;                                                                                         \
        _ss << msg;                                                                                                    \
        EBCpp::EBLogger::getInstance()->log(_ss);                                                                      \
    }

namespace EBCpp
{

class EBLogger : public EBObject<EBLogger>
{
public:
    static EBObjectPointer<EBLogger>& getInstance()
    {
        if( instance == nullptr )
        {
            instance = EBObjectPointer<EBLogger>(new EBLogger());
        }

        return instance;
    }

    void log(const std::stringstream & message)
    {
        std::lock_guard<std::mutex> guard(mutex);

        std::cout << message.str() << std::endl;
    }

private:
    EBLogger()
    {
    }

    static inline EBObjectPointer<EBLogger> instance = EBObjectPointer<EBLogger>(nullptr);
    std::mutex mutex;
};

} // namespace EBCpp
