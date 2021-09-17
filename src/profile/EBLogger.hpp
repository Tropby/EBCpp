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

#define EB_LOG(type, msg)                                                                                              \
    {                                                                                                                  \
        std::stringstream _ss;                                                                                         \
        _ss << msg;                                                                                                    \
        EBCpp::EBLogger::getInstance()->log(type, _ss);                                                                \
    }

namespace EBCpp
{

/**
 * @brief Logger class that handles all log calls
 *
 */
class EBLogger : public EBObject<EBLogger>
{
public:
    /**
     * @brief Log type.
     *        The logger can filter logs to create different logs
     *        for different programm states (Debug / Release)
     *
     */
    enum LOG_TYPE
    {
        LOG_DEBUG = 1,
        LOG_PROFILE = 2,
        LOG_WARNING = 4,
        LOG_ERROR = 8,
        LOG_CRITICAL = 16
    };

    /**
     * @brief Get the Singelton Instance object
     *
     * @return EBObjectPointer<EBLogger>&
     */
    static EBObjectPointer<EBLogger>& getInstance()
    {
        if (instance == nullptr)
        {
            instance = EBObjectPointer<EBLogger>(new EBLogger());
        }

        return instance;
    }

    /**
     * @brief Log a message to the current Log-Methods
     *
     * @param type Type of the log message
     * @param message Message that should be logged
     */
    void log(const LOG_TYPE type, const std::stringstream& message)
    {
        std::string strType = "UNKNOWN";
        switch (type)
        {
            LOG_TYPE::LOG_DEBUG:
            {
                strType = "DEBUG";
            }
            break;
            LOG_TYPE::LOG_PROFILE : break;
            LOG_TYPE::LOG_WARNING : break;
            LOG_TYPE::LOG_ERROR : break;
            LOG_TYPE::LOG_CRITICAL : break;
        }

        std::lock_guard<std::mutex> guard(mutex);

        /// TODO: Use LogFile and other classes to send log messages to sinks
        std::cout << message.str() << std::endl;
    }

private:
    /**
     * @brief Construct a new EBLogger object
     *
     */
    EBLogger()
    {
    }

    static inline EBObjectPointer<EBLogger> instance = EBObjectPointer<EBLogger>(nullptr);
    std::mutex mutex;
    LOG_TYPE typesToLog;
};

} // namespace EBCpp
