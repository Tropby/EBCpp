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

#include <list>

#include "profile/EBLogger.hpp"

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
class EBEventLoop : public EBObject<EBEventLoop>
{
public:
    /**
     * @brief Construct a new EBEventLoop object
     *
     * @param parent Parent of the event loop
     */
    EBEventLoop() : EBObject(), semaphore(createObject<EBSemaphore>(0)), closed(false){}

    /**
     * @brief Get the main instance (Main Loop)
     *
     * @return EBEventLoop& main event loop
     */
    static EBObjectPointer<EBEventLoop>& getInstance()
    {
        if( instance == nullptr )
        {
            instance = EBCreate<EBEventLoop>();
        }
        return instance;
    }

    /**
     * @brief A connection calls emit to emit a slot call to the event loop
     *
     * @param slot Slot that should be called
     */
    void emit(EBObjectPointer<EBSlotCall>& slot)
    {
        mutex.lock();
        events.push_back(slot);
        mutex.unlock();
        semaphore->release();
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
            EBObjectPointer<EBSlotCall>& slot = events.front();
            mutex.unlock();

            if(slot.isValid())
                slot->call();

            mutex.lock();
            events.pop_front();
            mutex.unlock();
        }
        EBObjectWatch::destroyObjects();
    }

    /**
     * @brief Starts a loop that processes all signals and ends only if exit is called
     *
     */
    void exec()
    {        
        EBUtils::setThreadName(EBString("EBEventLoop 0x") + EBUtils::intToHex(reinterpret_cast<long long>(this)));
        while (!closed)
        {
            semaphore->acquire();
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

        // Add a nullptr slot call to release the semaphore and exit the application
        EBCpp::EBObjectPointer<EBCpp::EBSlotCall> slot;
        emit(slot);
    }

    int getCount()
    {
        return events.size();
    }

private:
    std::list< EBObjectPointer<EBSlotCall> > events;
    std::mutex mutex;
    EBObjectPointer<EBSemaphore> semaphore;
    bool closed;

    static inline EBObjectPointer<EBEventLoop> instance = nullptr;
};

} // namespace EBCpp
