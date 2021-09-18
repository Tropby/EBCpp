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
#include "EBObject.hpp"
#include "profile/EBProfile.hpp"

#define EB_SIGNAL(signalName) EBCpp::EBEvent<> signalName
#define EB_SLOT(slotName) void slotName(EBCpp::EBObjectPointer<EBCpp::EBObject<EBCpp::EBObjectBase>> sender)
#define EB_EMIT(signalName)                                                                                            \
    {                                                                                                                  \
        EBObjectPointer<EBObject<EBObjectBase>> _cast = cast<EBObject<EBObjectBase>>();                                \
        signalName.emit(_cast);                                                                                        \
    }

#define EB_SLOT_WITH_ARGS(slotName, args...)                                                                           \
    void slotName(EBCpp::EBObjectPointer<EBCpp::EBObject<EBCpp::EBObjectBase>> sender, args)    
#define EB_EMIT_WITH_ARGS(signalName, args...)                                                                         \
    {                                                                                                                  \
        EBObjectPointer<EBObject<EBObjectBase>> _cast = cast<EBObject<EBObjectBase>>();                                \
        signalName.emit(_cast, args);                                                                                  \
    }

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
std::function<void(Types...)> bind(void (T::*f)(Types...), X object, std::integer_sequence<int, indices...>)
{
    return std::bind(f, object, EBPlaceholder<indices + 1>::ph...);
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
std::function<void(Types...)> bind(void (T::*f)(Types...), X object)
{
    return EBCpp::bind(f, object, std::make_integer_sequence<int, sizeof...(Types)>());
}
} // namespace EBCpp

namespace EBCpp
{

/**
 * @brief This objects handles an event
 *
 * @tparam args Parameter list of the event (eg. int, std::string, ...)
 */
template <typename... args>
class EBEvent : public EBObject<EBEvent<args...>>
{
public:
    /**
     * @brief Construct a new EBEvent object
     *
     * @param parent The parent can be set if the event is not an instance of the object that emits it
     */
    EBEvent() : EBObject<EBEvent<args...>>()
    {
    }

    /**
     * @brief Destroy the EBEvent object
     *
     */
    ~EBEvent()
    {
        // Destroy Event connections
        connections.clear();
    }

    /**
     * @brief Connects an event to a callback method of the receiving object using an event loop
     *
     * @tparam X Type of the receiving object (automatic set)
     * @tparam T Type of the receiving object  (automatic set)
     * @tparam Types Parameters of the function that will receive the event (automatic set)
     * @param eventLoop The EBEventLoop the event is connected to
     * @param receiver The receiver instance (EBObject) of the event
     * @param function The function of the receiver object that is called if the event is emitted
     */
    template <class X, class T, class... Types>
    void connect(EBObjectPointer<EBEventLoop> eventLoop, X receiver, void (T::*function)(Types...))
    {
        auto t = EBCpp::bind(function, receiver);
        connections.push_back(this->template createObject<EBConnection<args...>>(eventLoop, receiver, t));
    }

     /**
     * @brief Connects an event to a callback method of the receiving object using the default event loop
     *
     * @tparam X Type of the receiving object (automatic set)
     * @tparam T Type of the receiving object  (automatic set)
     * @tparam Types Parameters of the function that will receive the event (automatic set)
     * @param receiver The receiver instance (EBObject) of the event
     * @param function The function of the receiver object that is called if the event is emitted
     */
    template <class X, class T, class... Types>
    void connect(X receiver, void (T::*function)(Types...))
    {
        connect(EBEventLoop::getInstance(), receiver, function);
    }

    /**
     * @brief Disconnects alll slots from this event
     * 
     */
    void disconnectAll()
    {
        connections.clear();
    }

    /**
     * @brief Disconnects an event from a callback method of the receiving object using an event loop
     *
     * @tparam X Type of the receiving object (automatic set)
     * @tparam T Type of the receiving object  (automatic set)
     * @tparam Types Parameters of the function that will receive the event (automatic set)
     * @param eventLoop The EBEventLoop the event is connected to
     * @param receiver The receiver instance (EBObject) of the event
     * @param function The function of the receiver object that is called if the event is emitted
     */
    template <class X, class T, class... Types>
    void disconnect(EBObjectPointer<EBEventLoop> eventLoop, X receiver, void (T::*function)(Types...))
    {
        for (EBObjectPointer<EBConnection<args...>>& con : connections)
        {
            /// TODO: Check if the functions are equal.
            auto t = EBCpp::bind(function, receiver);
            auto ct = con->getFunction();

            EBObjectPointer<EBObject<EBObjectBase>> r = receiver;
            EBObjectPointer<EBObject<EBObjectBase>> cr = con->getReceiver();

            EBObjectPointer<EBEventLoop> e = eventLoop;
            EBObjectPointer<EBEventLoop> ce = con->getEventLoop();

            if (cr == r && ce == e)
            {
                connections.remove(con);
                return;
            }
        }
    }

    /**
     * @brief Disconnects an event from a callback method of the receiving object using the default event loop
     *
     * @tparam X Type of the receiving object (automatic set)
     * @tparam T Type of the receiving object  (automatic set)
     * @tparam Types Parameters of the function that will receive the event (automatic set)
     * @param receiver The receiver instance (EBObject) of the event
     * @param function The function of the receiver object that is called if the event is emitted
     */
    template <class X, class T, class... Types>
    void disconnect(X receiver, void (T::*function)(Types...))
    {
        disconnect(EBEventLoop::getInstance(), receiver, function);
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
    void emit(EBObjectPointer<EBObject<EBObjectBase>>& sender, args... p)
    {
        for (EBObjectPointer<EBConnection<args...>>& c : connections)
        {
            c->emit(sender, p...);
        }
    }

private:
    std::list<EBObjectPointer<EBConnection<args...>>> connections;

  
};

} // namespace EBCpp