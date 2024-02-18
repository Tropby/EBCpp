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

#include <functional>
#include <list>
#include <memory>

#include "EBEventLoop.hpp"
#include "EBObject.hpp"

namespace EBCpp
{

/**
 * @brief Class to connect between a signal and a slot
 *
 * @tparam args Argument list of the signal
 */
template <typename... args>
class EBConnection : public EBObject<EBConnection<args...>>
{
public:
    /**
     * @brief Construct a new EBConnection object
     *
     * @param eventLoop Event loop that will handle the slot calls
     * @param receiver Object that will receive the emitted signals
     * @param function Function to call
     */
    EBConnection(EBObjectPointer<EBEventLoop> eventLoop, EBObjectPointer<EBObject<EBObjectBase>> receiver,
                 std::function<void(EBObjectPointer<EBObject<EBObjectBase>>, args...)> function) :
        EBObject<EBConnection<args...>>(),
        eventLoop(eventLoop), receiver(receiver), function(function)
    {
    }

    ~EBConnection()
    {
    }

    /**
     * @brief Emit a signal
     *
     * @param sender Object that emitted the signal
     * @param p Arguments for the call
     */
    void emit(EBObjectPointer<EBObject<EBObjectBase>>& sender, args... p)
    {
        std::function<void()> f = std::bind(function, sender, p...);
        EBObjectPointer<EBSlotCall> sc = this->template createObject<EBSlotCall>(sender, receiver, f);
        eventLoop->emit(sc);
    }

    /**
     * @brief Get the Event Loop object
     * 
     * @return const EBObjectPointer<EBEventLoop>& 
     */
    const EBObjectPointer<EBEventLoop>& getEventLoop() const
    {
        return eventLoop;
    }

    /**
     * @brief Get the Receiver object
     * 
     * @return const EBObjectPointer<EBObject<EBObjectBase>>& 
     */
    const EBObjectPointer<EBObject<EBObjectBase>>& getReceiver() const
    {
        return receiver;
    }

    /**
     * @brief Get the Function object
     * 
     * @return const std::function<void(EBObjectPointer<EBObject<EBObjectBase>>, args...)>& 
     */
    const std::function<void(EBObjectPointer<EBObject<EBObjectBase>>, args...)>& getFunction() const
    {
        return function;
    }

private:
    //! The Event loop that will handle the signals emitted thought this
    //! connection
    EBObjectPointer<EBEventLoop> eventLoop;
    EBObjectPointer<EBObject<EBObjectBase>> receiver;
    std::function<void(EBObjectPointer<EBObject<EBObjectBase>>, args...)> function;
};

} // namespace EBCpp