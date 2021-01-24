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

#include "../src/EBEventLoop.hpp"
#include "../src/EBTimer.hpp"

using namespace std;
using namespace EBCpp;

/**
 * @brief Example to show the function of an timer
 * 
 */
class ExampleTimer : public EBObject
{
public:
	/**
	 * @brief Construct a new Example Timer object
	 * 
	 * @param parent Parent of this object
	 */
	ExampleTimer(EBObject* parent) : 
        EBObject(parent),
        timer(this)
	{
		timer.timeout.connect(*this, &ExampleTimer::timeout);
		timer.start( 1000 );
	}

private:
    EBTimer timer;

	/**
	 * @brief EB_SLOT timeout
	 * 
	 * Will be called by the timer for each timeout that
	 * occures.
	 * 
	 */
	EB_SLOT(timeout)
	{
		static int i = 0;
		cout << "timeout " << i++ << endl;

		if( i == 5 )
			EBEventLoop::getInstance().exit();
	}

};

/**
 * @brief Main from the timer example
 * 
 * @return int Exit code (0)
 */
int main()
{
	ExampleTimer tt(nullptr);
    EBEventLoop::getInstance().exec();
    return 0;
}