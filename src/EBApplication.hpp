/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten (Tropby)
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
 *  Created on: 2022-06-17
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBObject.hpp"
#include "EBList.hpp"
#include "EBString.hpp"
#include "EBEventLoop.hpp"

namespace EBCpp
{

class EBApplication : public EBObject<EBApplication>
{
public:
    static void setArguments( int argc, char ** argv )
    {
        for( int i = 0; i < argc; i++ )
        {

        }
    }

private:
    EBApplication(){}
    EBList<EBString> arguments;

};


} // namespace EBCpp

void EBInit();
void EBShutdown();

int main(int argc, char ** argv )
{
    EBCpp::EBApplication::setArguments(argc, argv);

    EBInit();
    EBCpp::EBEventLoop::getInstance()->exec();
    EBShutdown();

    return 0;
}