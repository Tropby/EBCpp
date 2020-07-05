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


#ifndef EVENT_H
#define EVENT_H

#include <functional>
#include <list>
#include <memory>

#include "ebconfig.h"
#include "ebeventloop.h"

#define EB_SIGNAL(name, args...) 	EBEvent<args> name
#define EB_SLOT						/* for readability */

namespace EBCpp{

template <typename... args>
class EBEvent;

template <typename... args>
class EBEventFunction
{
public:
    EBEventFunction( std::function<void(args...)> func, EBEventLoop & eventLoop )
        : eventLoop( eventLoop ), function( func )
    {

    }

    void call( args... p )
    {
        std::function<void()> f = std::bind( function, p... );
        eventLoop.addEvent(f);
    }

private:
    EBEventLoop & eventLoop;
    std::function<void(args...)> function;
};

template <typename... args>
class EBEvent
{
public:
    EBEvent(){}

    void connect( std::function<void(args...)> func )
    {
        std::shared_ptr< EBEventFunction<args...> > f = std::make_shared<EBEventFunction<args...>>( func, EBEventLoop::getMainLoop() );
        functions.push_back( f );
    }

    void connect( EBEventLoop & eventLoop, std::function<void(args...)> func )
    {
        std::shared_ptr< EBEventFunction<args...> > f = std::make_shared<EBEventFunction<args...>>( func, eventLoop );
        functions.push_back( f );
    }

    void emit( args... p ) // Emit and Wait for finished
    {
        for( auto it : functions )
        {
            it->call(p...);
        }
    }

private:
    std::list< std::shared_ptr< EBEventFunction<args...> > > functions;
};

}

#endif // EVENT_H
