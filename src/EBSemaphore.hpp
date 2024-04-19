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

#include <cinttypes>
#include <condition_variable>
#include <mutex>

#include "EBObject.hpp"

namespace EBCpp
{

/**
 * @brief Semaphore implementation for EBCpp
 *
 */
class EBSemaphore : public EBObject<EBSemaphore>
{
public:
    /**
     * @brief Construct a new EBSemaphore object
     *
     * @param startCount Number of tokens the semaphore can provide after the creation
     */
    EBSemaphore(uint32_t startCount) : EBObject(), tokens(startCount)
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
        while (tokens == 0)
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
        {
            std::lock_guard<std::mutex> lock(mutex);
            tokens++;
        }
        condition.notify_one();
    }

    /**
     * @brief Clears the semaphore and sets token count to zero
     *
     */
    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex);
        tokens = 0;
    }

private:
    uint32_t tokens;
    std::mutex mutex;
    std::condition_variable condition;
};

} // namespace EBCpp
