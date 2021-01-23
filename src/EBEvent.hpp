#pragma once

#include <functional>
#include <list>
#include <memory>

#include "EBObject.hpp"
#include "EBEventLoop.hpp"
#include "EBConnection.hpp"

#define EB_SIGNAL(signalName) EBCpp::EBEvent<> signalName
#define EB_SLOT(slotName) void slotName(EBCpp::EBObject* sender)
#define EB_EMIT(signalName) signalName.emit(*this);

#define EB_SLOT_WITH_ARGS(slotName, args...) void slotName(EBCpp::EBObject* sender, args)
#define EB_EMIT_WITH_ARGS(signalName, args...) signalName.emit(*this, args);
#define EB_SIGNAL_WITH_ARGS(signalName, args...) EBCpp::EBEvent<args> signalName

namespace EBCpp
{
    template<int N>
    struct EBPlaceholder { static EBPlaceholder ph; };

    template<int N>
    EBPlaceholder<N> EBPlaceholder<N>::ph;
}

namespace std {
    template<int N>
    struct is_placeholder< EBCpp::EBPlaceholder< N > > : std::integral_constant<int, N> { };
}

namespace EBCpp
{
    template<class T, class...Types, class X, int... indices>
    std::function<void (Types...)> bind( void (T::*f) (Types...), X& object, std::integer_sequence<int, indices...> ) 
    {
        return std::bind(f, &object, EBPlaceholder<indices+1>::ph...);
    }

    template<class T, class...Types, class X>
    std::function<void (Types...)> bind( void (T::*f) (Types...), X& object )
    {   
        
        return EBCpp::bind( f, object, std::make_integer_sequence<int, sizeof...(Types)>() );       
    } 
}

namespace EBCpp
{

template <typename... args>
class EBEvent
{
public:
    template<class X, class T, class...Types>
    void connect( EBEventLoop& eventLoop, X& receiver, void (T::*function) (Types...) )
    {
        auto f = EBCpp::bind( function, receiver );
        auto s = std::make_shared< EBConnection<args...> >( eventLoop, receiver, f);
        connections.push_back( s );
    }

    template<class X, class T, class...Types>
    void connect( X& receiver, void (T::*function) (Types...) )
    {
        connect( EBEventLoop::getInstance(), receiver, function );
    }

    void emit( EBObject& sender, args... p )
    {
        for( auto c : connections )
        {
            c->emit(sender, p...);
        }
    }

private:
    std::list< std::shared_ptr<EBConnection<args...> > > connections;
    
};


}