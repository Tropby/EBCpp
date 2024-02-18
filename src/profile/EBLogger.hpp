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

#include "../EBObject.hpp"
#include "../EBUtils.hpp"
#include "EBLog.hpp"
#include "EBLogConsole.hpp"

#include <mutex>
#include <sstream>
#include <regex>

#define EB_LOG_INFO(msg) EB_LOG(EBCpp::EBLogger::LOG_INFO, msg)
#define EB_LOG_DEBUG(msg) EB_LOG(EBCpp::EBLogger::LOG_DEBUG, msg)
#define EB_LOG_PROFILE(msg) EB_LOG(EBCpp::EBLogger::LOG_PROFILE, msg)
#define EB_LOG_WARNING(msg) EB_LOG(EBCpp::EBLogger::LOG_WARNING, msg)
#define EB_LOG_ERROR(msg) EB_LOG(EBCpp::EBLogger::LOG_ERROR, msg)
#define EB_LOG_CRITICAL(msg) EB_LOG(EBCpp::EBLogger::LOG_CRITICAL, msg)

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
        LOG_CRITICAL = 16,
        LOG_INFO = 32
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

    static void addLogger(EBObjectPointer<EBLog> log)
    {
        getInstance()->logger.push_back(log);
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
        case LOG_DEBUG:
            strType = "DEBUG";
            break;
        case LOG_TYPE::LOG_PROFILE:
            strType = "PROFILE";
            break;
        case LOG_TYPE::LOG_WARNING:
            strType = "WARNING";
            break;
        case LOG_TYPE::LOG_ERROR:
            strType = "ERROR";
            break;
        case LOG_TYPE::LOG_CRITICAL:
            strType = "CRITICAL";
            break;
        case LOG_TYPE::LOG_INFO:
            strType = "INFO";
            break;
        }

        std::lock_guard<std::mutex> guard(mutex);

        std::stringstream msg;
        msg << EBUtils::currentDateTimeString() << " " << strType << " [tid: " << std::this_thread::get_id()
            << ", tname: '" << EBUtils::getThreadName() << "', uptime: " << EBUtils::uptime() << "] " << message.str();

        // Add Logger if no logger is available!
        if (logger.empty())
        {
            logger.push_back(EBObjectPointer<EBLog>(new EBLogConsole()));
        }

        std::string strMsg = msg.str();

        strMsg = EBUtils::replaceString(strMsg, "\n", "\\n");
        strMsg = EBUtils::replaceString(strMsg, "\r", "\\r");

        for (auto log : logger)
        {
            log->log(strMsg);
        }
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

    std::list<EBObjectPointer<EBLog>> logger;

    std::mutex mutex;
};

} // namespace EBCpp
