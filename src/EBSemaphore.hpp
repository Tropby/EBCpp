/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
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

#include <cinttypes>
#include <mutex>
#include <condition_variable>

#include "EBObject.hpp"

namespace EBCpp
{

/**
 * @brief Semaphore implementation for EBCpp
 * 
 */
class EBSemaphore : public EBObject
{
public:    
    /**
     * @brief Construct a new EBSemaphore object
     * 
     * @param startCount Number of tokens the semaphore can provide after the creation
     * @param parent Parent EBCpp object
     */
    EBSemaphore(uint32_t startCount, EBObject* parent = nullptr) : 
        EBObject(parent),
        tokens(startCount)
    {
    }

    /**
     * @brief Acquires a token from the semaphore
     * 
     * Acquires a token from the semaphore, if no token is available this 
     * function locks until a token is available
     */
    void acquire()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(tokens == 0)
        {
            condition.wait(lock);
        }
        tokens--;
    }

    /**
     * @brief Releases a token to the semaphore
     * 
     */
    void release()
    {
        std::unique_lock<std::mutex> lock(mutex);
        tokens++;
        condition.notify_one();
    }

    /**
     * @brief Clears the semaphore and sets token count to zero
     * 
     */
    void clear()
    {
        tokens = 0;
    }

private:
    uint32_t tokens;
    std::mutex mutex;
    std::condition_variable condition;
};

}
