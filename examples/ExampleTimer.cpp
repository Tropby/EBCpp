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

#include "../src/EBApplication.hpp"
#include "../src/EBEventLoop.hpp"
#include "../src/EBTimer.hpp"
#include "../src/profile/EBLogger.hpp"
#include "../src/profile/EBProfile.hpp"

using namespace std;
using namespace EBCpp;

/**
 * @brief Example to show the function of an timer
 *
 */
class ExampleTimer : public EBObject<ExampleTimer>
{
public:
    /**
     * @brief Construct a new Example Timer object
     *
     */
    ExampleTimer() 
    {
        EB_PROFILE_FUNC();

        timer.timeout.connect(this, &ExampleTimer::timeout);
        timer.start(1000);
    }

    /**
     * @brief Destroy the Example Timer object
     * 
     */
    ~ExampleTimer()
    {
        timer.timeout.disconnect(this, &ExampleTimer::timeout);
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
        EB_PROFILE_FUNC();

        static int i = 0;
        EB_LOG(EBLogger::LOG_DEBUG, "timeout " << ++i);

        if (i == 5)
            EBEventLoop::getInstance()->exit();
    }
};

EB_APPLICATION(ExampleTimer);
