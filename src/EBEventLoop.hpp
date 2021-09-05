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

#include <list>

#include "EBObject.hpp"
#include "EBSemaphore.hpp"
#include "EBSlotCall.hpp"
#include "EBUtils.hpp"

namespace EBCpp
{

/**
 * @brief Event handling class
 *
 */
class EBEventLoop : public EBObject
{
public:
    /**
     * @brief Construct a new EBEventLoop object
     *
     * @param parent Parent of the event loop
     */
    EBEventLoop(EBObject* parent) : EBObject(parent), semaphore(0, this)
    {
    }

    /**
     * @brief Get the main instance (Main Loop)
     *
     * @return EBEventLoop& main event loop
     */
    static EBEventLoop& getInstance()
    {
        static std::unique_ptr<EBEventLoop> instance;
        if (!instance)
        {
            instance = std::unique_ptr<EBEventLoop>(new EBEventLoop(nullptr));
        }
        return *instance;
    }

    /**
     * @brief A connection calls emit to emit a slot call to the event loop
     *
     * @param slot Slot that should be called
     */
    void emit(std::shared_ptr<EBSlotCall> slot)
    {
        mutex.lock();
        events.push_back(slot);
        mutex.unlock();
        semaphore.release();
    }

    /**
     * @brief Processes all events that are available to the event loop
     *
     */
    void processEvents()
    {
        while (events.size())
        {
            mutex.lock();
            std::shared_ptr<EBSlotCall> slot = events.front();
            mutex.unlock();

            slot->call();

            mutex.lock();
            events.pop_front();
            mutex.unlock();
        }
    }

    /**
     * @brief Starts a loop that processes all signals and ends only if exit is called
     *
     */
    void exec()
    {
        EBUtils::setThreadName("EBEventLoop 0x" + EBUtils::intToHex(reinterpret_cast<long long>(this)));
        closed = false;
        while (!closed)
        {
            semaphore.acquire();
            processEvents();
        }
    }

    /**
     * @brief Exit the loop
     * @see exec()
     *
     */
    void exit()
    {
        closed = true;
    }

private:
    std::list< std::shared_ptr<EBSlotCall> > events;
    std::mutex mutex;
    EBSemaphore semaphore;
    bool closed;
};

} // namespace EBCpp
