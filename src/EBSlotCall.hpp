#pragma once

#include <string>
#include <memory>
#include <thread>
#include <functional>

#include "EBApplication.hpp"
#include "EBObject.hpp"

namespace EBCpp
{

class EBSlotCall : public EBObject
{
public:
    EBSlotCall(EBObject& sender, EBObject& receiver, std::function<void()> f) :
        sender(&sender),
        receiver(&receiver),
        function(f)
    {
    }

    void call()
    {
        if( EBApplication::isValidObject(sender) && EBApplication::isValidObject(receiver) )
        {
            function();
        }
    }

private:
    EBObject * sender;
    EBObject * receiver;
    std::function<void()> function;
};

}