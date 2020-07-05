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

#include "ebtimer.h"

using namespace EBCpp;

EBTimer::EBTimer() : timerRunning(false), threadRunning(true), autoRetrigger(false), time(time), thread( std::bind( &EBTimer::run, this ) )
{
}

void EBTimer::trigger()
{
	semaphore.release();
}

EBTimer::~EBTimer()
{
	threadRunning = false;
	trigger();
	thread.join();
}

void EBTimer::stop()
{
	this->timerRunning = false;
	this->autoRetrigger = false;
	this->semaphore.clear();
}

void EBTimer::startSingleShot(uint32_t time)
{
	this->timerRunning = true;
	this->autoRetrigger = false;
	this->time = time;
	trigger();
}

void EBTimer::run()
{
    while( threadRunning )
    {
		semaphore.acquire();

    	if( threadRunning && timerRunning )
    		std::this_thread::sleep_for(std::chrono::milliseconds(time));

    	if( threadRunning && timerRunning )
    		timeout.emit();

    	if( autoRetrigger && timerRunning )
    		trigger();
    }
}

void EBTimer::start(uint32_t time)
{
	this->timerRunning = true;
	this->autoRetrigger = true;
	this->time = time;
	trigger();
}
