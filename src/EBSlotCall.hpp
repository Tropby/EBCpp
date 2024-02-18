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
#include <memory>
#include <string>
#include <thread>

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
class EBSlotCall : public EBObject<EBSlotCall>
{
public:
    /**
     * @brief Construct a new EBSlotCall object
     *
     * @param sender The emitting object
     * @param receiver The object that slot will be called
     * @param f The method pointer of the receiver object that will be executed
     */
    EBSlotCall(EBObjectPointer<EBObject<EBObjectBase>>& sender, EBObjectPointer<EBObject<EBObjectBase>>& receiver,
               std::function<void()>& f) :       
        sender(sender), receiver(receiver), function(f)
    {
    }

    /**
     * @brief The event loop calls this slot and executed the function
     */
    void call()
    {
        function();
    }

private:
    EBObjectPointer<EBObject<EBObjectBase>> sender;
    EBObjectPointer<EBObject<EBObjectBase>> receiver;
    std::function<void()> function;
};

} // namespace EBCpp