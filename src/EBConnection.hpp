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