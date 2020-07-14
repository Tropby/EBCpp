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
 *  Created on: Jul 3, 2020
 *      Author: Carsten (Tropby)
 */


#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <list>
#include <thread>
#include <functional>
#include <mutex>

#include "ebconfig.h"
#include "ebsemaphore.h"

namespace EBCpp{

class EBEventLoop
{
public:
    EBEventLoop();

    int exec();
    void addEvent( std::function<void()> & func );

    void quit();

    static EBEventLoop &getMainLoop();

private:
    static EBEventLoop mainLoop;

    bool running;
    std::thread thread;
    std::list<std::function<void()>> eventList;
    std::mutex mutex;

    EBSemaphore semaphore;
};

}

#endif // EVENTLOOP_H
