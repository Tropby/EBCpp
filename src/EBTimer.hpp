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

#include <functional>
#include <thread>

#include "EBEvent.hpp"
#include "EBSemaphore.hpp"

namespace EBCpp
{

/**
 * @brief The EBTimer can provide times events
 */
class EBTimer : public EBObject
{
public:
    /**
     * @brief Construct a new EBTimer object
     *
     * @param parent Parent of the EBTimer object
     */
    EBTimer(EBObject* parent) : EBObject(parent), timerRunning(true), singleShot(false), time(-1), thread(nullptr)
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
        while (timerRunning)
        {
            // Wait for the timer run out or the timer is canceled
            {
                std::unique_lock<std::mutex> lock(mWait);
                cvWait.wait_for(lock, std::chrono::milliseconds(time), [&] { return !timerRunning; });
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
