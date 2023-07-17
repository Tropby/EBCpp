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

#include "../src/EBObject.hpp"
#include "../src/EBEvent.hpp"

using namespace EBCpp;

class ExampleSignal : public EBObject<ExampleSignal>
{
public:
    ExampleSignal()
    {
    }

    EB_SIGNAL(helloWorldCalled);

    void helloWorld()
    {
        EB_EMIT(helloWorldCalled);
    }

private:
    
};

class ExampleSlot : public EBObject<ExampleSlot>
{
public:
    ExampleSlot()
    {
    }

    EB_SLOT(helloWorld)
    {
        // Output "Hello World!" to the console.
        std::cout << "Hello World!" << std::endl;
                
        // End the event loop
        EBEventLoop::getInstance()->exit();
    }

private:
};

int main()
{
    // Setup the instances of the signal and slot example classes
    ExampleSignal signal;
    ExampleSlot slot;

    // Connect to the hello world signal to the hello world slot
    signal.helloWorldCalled.connect(&slot, &ExampleSlot::helloWorld);

    // Call the slot (this will add the call to the event loop)
    signal.helloWorld();

    // You can now disconnect the slot from the signal. This will not remove the call from the event loop.
    signal.helloWorldCalled.disconnect(&slot, &ExampleSlot::helloWorld);

    // Start the event loop (ExampleSlot::helloWorld will be called)
    EBEventLoop::getInstance()->exec();
}