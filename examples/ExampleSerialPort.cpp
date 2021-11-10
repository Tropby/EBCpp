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

#include <functional>
#include <iostream>
#include <string>

#include "../src/EBEvent.hpp"
#include "../src/EBEventLoop.hpp"
#include "../src/EBTimer.hpp"
#include "../src/serialport/EBSerialPort.hpp"

/**
 * @brief Example to show the function of the TCP server
 *
 */
class ExampleSerialPort : public EBCpp::EBObject<ExampleSerialPort>
{
public:
    /**
     * @brief Construct a new Example Tcp Server object
     *
     */
    ExampleSerialPort()
    {
        timer.timeout.connect(this, &ExampleSerialPort::timeout);

        serialPort.readReady.connect(this, &ExampleSerialPort::readReady);
        serialPort.setPort(4);
        if (!serialPort.open(EBCpp::EBSerialPort::READ_WRITE))
            EB_EXCEPTION("Can not open Comport!");

        timer.start(1000);
    }

    EB_SLOT(timeout)
    {
        serialPort.write("Test\n");
    }

    EB_SLOT(readReady)
    {
        if( serialPort.canReadLine() )
        {
            EB_LOG_DEBUG("Received Data: " << serialPort.readLine());
        }
    }

private:
    EBCpp::EBSerialPort serialPort;
    EBCpp::EBTimer timer;
};

int main()
{
    ExampleSerialPort exampleSerialPort;
    EBCpp::EBEventLoop::getInstance()->exec();
}
