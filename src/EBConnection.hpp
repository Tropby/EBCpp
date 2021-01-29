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
#include <list>

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
class EBConnection : public EBObject
{
public:
    /**
     * @brief Construct a new EBConnection object
     *
     * @param eventLoop Event loop that will handle the slot calls
     * @param receiver Object that will receive the emitted signals
     * @param function Function to call
     */
    EBConnection(EBEventLoop& eventLoop, EBObject& receiver, std::function<void(EBObject*, args...)> function) :
        EBObject(&receiver), eventLoop(&eventLoop), receiver(&receiver), function(function)
    {
    }

    /**
     * @brief Emit a signal
     *
     * @param sender Object that emitted the signal
     * @param p Arguments for the call
     */
    void emit(EBObject* sender, args... p)
    {
        if (EBObject::isValidObject(eventLoop))
        {
            std::function<void()> f = std::bind(function, sender, p...);
            (*eventLoop).emit( std::make_shared< EBSlotCall>(sender, receiver, f));
        }
    }

private:
    //! The Event loop that will handle the signals emitted thought this
    //! connection
    EBEventLoop* eventLoop;
    EBObject* receiver;
    std::function<void(EBObject*, args...)> function;
};

} // namespace EBCpp