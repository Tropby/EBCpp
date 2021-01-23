#pragma once

#include <cinttypes>
#include <mutex>
#include <condition_variable>

#include "EBObject.hpp"

namespace EBCpp
{

class EBSemaphore : public EBObject
{
public:    
    EBSemaphore(uint32_t startCount) : tokens(startCount)
    {
    }

    void acquire()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while(tokens == 0)
        {
            condition.wait(lock);
        }
        tokens--;
    }

    void release()
    {
        std::unique_lock<std::mutex> lock(mutex);
        tokens++;
        condition.notify_one();
    }

    void clear()
    {
        tokens = 0;
    }

private:
    uint32_t tokens;
    std::mutex mutex;
    std::condition_variable condition;
};

}
