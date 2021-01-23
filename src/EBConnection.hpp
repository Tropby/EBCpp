#pragma once

#include <functional>
#include <list>

#include "EBObject.hpp"
#include "EBEventLoop.hpp"

namespace EBCpp
{

template <typename... args>
class EBConnection : public EBObject
{
public:
    EBConnection( EBEventLoop& eventLoop, EBObject& receiver, std::function<void(EBObject*, args...)> function ) : 
        eventLoop(&eventLoop),
        receiver(&receiver),
        function(function)
    {
        std::cout << std::hex << "Connection to " << reinterpret_cast<long long>(this->receiver) << " established!" << std::endl;

    }

    void emit(EBObject& sender, args... p)
    {
        if( EBApplication::isValidObject(eventLoop) )
        {
            std::function<void()> f = std::bind(function, &sender, p...);
            (*eventLoop).emit(std::make_shared<EBSlotCall>(sender, *receiver, f));
        }        
    }   

private:
    EBEventLoop * eventLoop;
    EBObject* receiver;
    std::function<void(EBObject*, args...)> function;
};

}