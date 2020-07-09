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

#include <iostream>

#include "../../src/ebtimer.h"

using namespace std;
using namespace EBCpp;

class TimerTest
{
public:
	TimerTest()
	{
		timer.timeout.connect(std::bind( &TimerTest::timeout, this ));
		timer.start( 1000 );
	}

private:
    EBTimer timer;

	void timeout()
	{
		static int i = 0;
	    cout << "foo2 " << i++ << endl;
	}

};

int mainTimerTest()
{
	TimerTest tt;

    return EBEventLoop::getMainLoop().finished();
}