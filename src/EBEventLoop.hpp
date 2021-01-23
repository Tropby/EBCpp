#pragma once

#include <list>

#include "EBObject.hpp"
#include "EBSemaphore.hpp"
#include "EBSlotCall.hpp"

namespace EBCpp
{

class EBEventLoop : public EBObject
{
public:
    EBEventLoop() : semaphore(0)
    {

    }

    static EBEventLoop & getInstance()
    {
        static std::unique_ptr<EBEventLoop> instance;
        if( !instance )
        {
            instance = std::unique_ptr<EBEventLoop>(new EBEventLoop());
        }
        return *instance;
    }

    void emit(std::shared_ptr<EBSlotCall> slot)
    {
        events.push_back( slot );
        semaphore.release();
    }

    void processEvents()
    {
        while( events.size() )
        {
            auto slot = events.front();
            slot->call();
            events.pop_front();
        }
    }

    void exec()
    {
        closed = false;
        while( !closed )
        {
            semaphore.acquire();
            processEvents();
        }
    }
    
    void exit()
    {
        closed = true;
    }

private:
    std::list< std::shared_ptr<EBSlotCall> > events;
    EBSemaphore semaphore;
    bool closed;
};

}
