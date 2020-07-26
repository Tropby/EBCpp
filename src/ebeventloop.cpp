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

#include "ebeventloop.h"

#include <iostream>

using namespace EBCpp;

EBEventLoop EBEventLoop::mainLoop;

EBEventLoop::EBEventLoop() :
		running(true)
{

}

int EBEventLoop::exec()
{
	while (running)
	{
		semaphore.acquire();
		auto f = eventList.front();
		f();
		eventList.pop_front();
	}

	thread.join();
	return 0;
}

void EBEventLoop::addEvent(std::function<void()> &func)
{
	eventList.push_back(func);
	semaphore.release();
}

void EBEventLoop::quit()
{
	running = false;
	semaphore.release();
}

EBEventLoop& EBEventLoop::getMainLoop()
{
	return mainLoop;
}
