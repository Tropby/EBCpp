#pragma once

#include <algorithm>
#include <memory>
#include <list>
#include <iostream>

namespace EBCpp
{

class EBObject;

class EBApplication
{
public:
    static EBApplication & getInstance()
    {
        static std::unique_ptr<EBApplication> instance;
        if( !instance )
        {
            instance = std::unique_ptr<EBApplication>(new EBApplication());
        }
        return *instance;
    }

    static void objectCreated( EBObject & object)
    {
        EBApplication & app = EBApplication::getInstance();

        bool found = (std::find(app.livingObjects.begin(), app.livingObjects.end(), &object) != app.livingObjects.end());
        if(!found)
            app.livingObjects.push_back(&object);
    }

    static void objectDestroyed( EBObject & object )
    {
        EBApplication & app = EBApplication::getInstance();
        app.livingObjects.remove( &object );
    }

    static bool isValidObject( EBObject & object )
    {
        return EBApplication::isValidObject(&object);
    }

    static bool isValidObject( EBObject * object )
    {
        EBApplication & app = EBApplication::getInstance();
        return (std::find(app.livingObjects.begin(), app.livingObjects.end(), object) != app.livingObjects.end());
    }

private:
    EBApplication()
    {

    }

    std::list< EBObject* > livingObjects;
};

}