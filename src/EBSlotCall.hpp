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

#include <string>
#include <memory>
#include <thread>
#include <functional>

#include "EBApplication.hpp"
#include "EBObject.hpp"

namespace EBCpp
{

/**
 * @brief Object to call a slot
 * 
 * Each time a signal is emitted, for every slot an EBSlotCall is creatred.
 * When the EventLoop calls this slot the EBSlotCall checks if the objects
 * provides to the call are valid and calls the slot.
 * 
 */
class EBSlotCall : public EBObject
{
public:
    /**
     * @brief Construct a new EBSlotCall object
     * 
     * @param sender The emitting object
     * @param receiver The object that slot will be called
     * @param f The method pointer of the receiver object that will be executed
     */
    EBSlotCall(EBObject * sender, EBObject * receiver, std::function<void()> f) :
        EBObject(nullptr),
        sender(sender),
        receiver(receiver),
        function(f)
    {
    }

    /**
     * @brief The event loop calls this slot and executed the function
     */
    void call()
    {
        if( EBApplication::isValidObject(sender) && EBApplication::isValidObject(receiver) )
        {
            function();
        }
    }

private:
    EBObject * sender;
    EBObject * receiver;
    std::function<void()> function;
};

}