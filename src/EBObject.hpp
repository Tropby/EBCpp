#pragma once

#include <string>
#include <memory>
#include <thread>
#include <typeinfo>

#include "EBApplication.hpp"

namespace EBCpp
{

class EBObject
{
public:
    EBObject()
    {      
        name = std::string(typeid(this).name()) + " - " + std::to_string( reinterpret_cast<long long>(this) );
        EBApplication::getInstance().objectCreated(*this);
        threadId = std::this_thread::get_id();

        std::cout << "Create EBObject " << std::hex << reinterpret_cast<long long>(this) << std::endl;
    }

    ~EBObject()
    {
        std::cout << "Destroy EBObject" << std::hex << reinterpret_cast<long long>(this) << std::endl;
        
        EBApplication::getInstance().objectDestroyed(*this);
    }

    std::string getName()
    {
        return name;
    }

private:
    std::string name;
    std::thread::id threadId;

};

}