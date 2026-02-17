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

#include <chrono>
#include <functional>
#include <memory>
#include <thread>

#include <unistd.h>

#include "EBEvent.hpp"
#include "EBSemaphore.hpp"

namespace EBCpp
{

class EBTimer;
typedef EBObjectPointer<EBTimer> EBTimerPtr;

/**
 * @brief The EBTimer can provide times events
 */
class EBTimer : public EBObject<EBTimer>
{
public:
    /**
     * @brief Construct a new EBTimer object
     *
     * @param parent Parent of the EBTimer object
     */
    EBTimer() : EBObject(), timerRunning(true), singleShot(false), time(-1), thread(nullptr)
    {
    }

    /**
     * @brief Destroy the EBTimer object
     *
     */
    ~EBTimer()
    {
        stop();
    }

    /**
     * @brief EB_SIGNAL timeout
     *
     * Emitted every time the timer ends
     */
    EB_SIGNAL(timeout);

    /**
     * @brief Stopps the timer
     */
    void stop()
    {
        if (!thread)
            return;
        {
            std::unique_lock<std::mutex> lock(mWait);
            this->timerRunning = false;
        }
        cvWait.notify_one();
        thread->join();
        delete thread;
        thread = nullptr;
    }

    /**
     * @brief Starts the timer for exactly one timeout
     *
     * @param time Timeout time in milliseconds
     */
    void startSingleShot(uint32_t time)
    {
        if (thread)
        {
            stop();
        }

        this->timerRunning = true;
        this->singleShot = true;
        this->time = time;
        thread = new std::thread(std::bind(&EBTimer::run, this));
    }

    /**
     * @brief Starts the timer for interval timeouts
     *
     * @param time Timeout time in milliseconds
     */
    void start(uint32_t time)
    {
        if (thread)
        {
            stop();
        }
        this->timerRunning = true;
        this->singleShot = false;
        this->time = time;
        thread = new std::thread(std::bind(&EBTimer::run, this));
    }

    /**
     * @brief Checks if the timer is currently running.
     *
     * @return true if the timer is running.
     * @return false if the timer is not running.
     */
    bool isRunning()
    {
        return timerRunning;
    }

private:
    bool timerRunning;
    bool singleShot;

    uint32_t time;
    std::thread* thread;

    mutable std::condition_variable cvWait;
    mutable std::mutex mWait;

    /**
     * @brief Method that runs the thread
     *
     */
    void run()
    {
        EBUtils::setThreadName(EBString( "EBTimer " ) + EBUtils::intToHex((long long)this, true));

        while (timerRunning)
        {
            // Wait for the timer run out or the timer is canceled
            {
                /// TODO: Check why the wait_for function will lock the time sometimes.

                std::unique_lock<std::mutex> lock(mWait);

                /// INFO: condition variable wait_for uses system clock and not steady clock for GCC < 10
                // cvWait.wait_for(lock, std::chrono::milliseconds(time), [&] { return !timerRunning; });

                //cvWait.wait_until(lock, std::chrono::steady_clock::now() + std::chrono::milliseconds(time),
                                  //[&] { return !timerRunning; });

                //std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(time));

                // C++ Sleep was not working correctly with older GCC <= 9 Versions on Embedded Systems
                usleep(time * 1000UL);
            }

            // Emit the timeout event if the timer is still running and the thead should not end
            if (timerRunning)
            {
                EB_EMIT(timeout);
            }

            // If you run a singleShot timer just stop the timer
            if (singleShot)
                timerRunning = false;
        }
    }
};

} // namespace EBCpp
