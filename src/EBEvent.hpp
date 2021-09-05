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
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include <functional>
#include <list>
#include <memory>

#include "EBConnection.hpp"
#include "EBEventLoop.hpp"

#define EB_SIGNAL(signalName) EBCpp::EBEvent<> signalName
#define EB_SLOT(slotName) void slotName(EBCpp::EBObject* sender)
#define EB_EMIT(signalName) signalName.emit(this)

#define EB_SLOT_WITH_ARGS(slotName, args...) void slotName(EBCpp::EBObject* sender, args)
#define EB_EMIT_WITH_ARGS(signalName, args...) signalName.emit(this, args)
#define EB_SIGNAL_WITH_ARGS(signalName, args...) EBCpp::EBEvent<args> signalName

namespace EBCpp
{
/**
 * @brief Automatic placeholder for slot generation
 *
 * @tparam N Number of the iterator
 */
template <int N>
struct EBPlaceholder
{
    static EBPlaceholder ph;
};

/**
 * @brief Placeholder iterator
 *
 * @tparam N Number of the iterator
 */
template <int N>
EBPlaceholder<N> EBPlaceholder<N>::ph;
} // namespace EBCpp

namespace std
{
/**
 * @brief Define the EBPlaceholder as std::placeholder
 *
 * @tparam N Number of the iterator
 */
template <int N>
struct is_placeholder<EBCpp::EBPlaceholder<N>> : std::integral_constant<int, N>
{
};
} // namespace std

namespace EBCpp
{
/**
 * @brief Bind a function to an slot with multiple placeholders
 *
 * @tparam T Type of the object that will receive the signal
 * @tparam Types Parameter list of the slot
 * @tparam X Object to bind the slot to
 * @tparam indices Indices for the placeholders
 * @param f Method to bind
 * @param object Object to bind
 * @return std::function<void (Types...)> Function pointer
 */
template <class T, class... Types, class X, int... indices>
std::function<void(Types...)> bind(void (T::*f)(Types...), X& object, std::integer_sequence<int, indices...>)
{
    return std::bind(f, &object, EBPlaceholder<indices + 1>::ph...);
}

/**
 * @brief Bind a function to an slot with multiple placeholders
 *
 * @tparam T Type of the object that will receive the signal
 * @tparam Types Parameter list of the slot
 * @tparam X Object to bind the slot to
 * @param f Method to bind
 * @param object Object to bind
 * @return std::function<void (Types...)> Function pointer
 */
template <class T, class... Types, class X>
std::function<void(Types...)> bind(void (T::*f)(Types...), X& object)
{

    return EBCpp::bind(f, object, std::make_integer_sequence<int, sizeof...(Types)>());
}
} // namespace EBCpp

namespace EBCpp
{

class EBObject;

/**
 * @brief This objects handles an event
 *
 * @tparam args Parameter list of the event (eg. int, std::string, ...)
 */
template <typename... args>
class EBEvent : EBObject
{
public:
    /**
     * @brief Construct a new EBEvent object
     *
     * @param parent The parent can be set if the event is not an instance of the object that emits it
     */
    EBEvent(EBObject* parent = nullptr) : EBObject(parent)
    {
    }

    /**
     * @brief Destroy the EBEvent object
     *
     */
    ~EBEvent()
    {
    }

    /**
     * @brief Connects a event to an callback method of the receiving object using an event loop
     *
     * @tparam X Type of the receiving object (automatic set)
     * @tparam T Type of the receiving object  (automatic set)
     * @tparam Types Parameters of the function that will receive the event (automatic set)
     * @param eventLoop The EBEventLoop the event is connected to
     * @param receiver The receiver instance (EBObject) of the event
     * @param function The function of the receiver object that is called if the event is emitted
     */
    template <class X, class T, class... Types>
    void connect(EBEventLoop& eventLoop, X& receiver, void (T::*function)(Types...))
    {
        connections.push_back(std::make_shared<EBConnection<args...>>( eventLoop, receiver, EBCpp::bind(function, receiver) ) );
    }

    /**
     * @brief Connects a event to an callback method of the receiving object using the default event loop
     *
     * @tparam X Type of the receiving object (automatic set)
     * @tparam T Type of the receiving object  (automatic set)
     * @tparam Types Parameters of the function that will receive the event (automatic set)
     * @param receiver The receiver instance (EBObject) of the event
     * @param function The function of the receiver object that is called if the event is emitted
     */
    template <class X, class T, class... Types>
    void connect(X& receiver, void (T::*function)(Types...))
    {
        connect(EBEventLoop::getInstance(), receiver, function);
    }

    /**
     * @brief Emits the event
     *
     * To emit an event use the EB_EMIT and EB_EMIT_WITH_ARGS macros.
     * @code
     *      EB_EMIT( timeout )
     * @endcode
     * @code
     *      int i = 12;
     *      EB_EMIT_WITH_ARGS( timeout, i )
     * @endcode
     *
     * @param sender
     * @param p
     */
    void emit(EBObject* sender, args... p)
    {
        for (std::shared_ptr<EBConnection<args...>> c : connections)
        {
            c->emit(sender, p...);
        }
    }

private:
    std::list< std::shared_ptr<EBConnection<args...>>> connections;
};

} // namespace EBCpp