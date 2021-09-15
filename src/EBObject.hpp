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

#include <list>
#include <memory>
#include <string>
#include <thread>
#include <typeinfo>
#include <mutex>
#include <algorithm>
#include <iostream>

#include "EBUtils.hpp"

namespace EBCpp
{

class EBObjectPointerBase;
template <class T>
class EBObjectPointer;

class EBObjectBase
{
public:
    EBObjectBase(std::string name) : name(name)
    {
        _counter++;
    }

    virtual ~EBObjectBase()
    {
        _counter--;    
    }

    template <class C, class... argList>
    static EBObjectPointer<C> createObject(argList... args)
    {
        C* object = new C(args...);
        objectList.push_back(object);
        return EBObjectPointer<C>(object);
    }

    static void destroyObjects()
    {
        for (EBObjectBase* ptr : objectToBeDestroyed)
            destroyObject(ptr);

        objectToBeDestroyed.clear();
    }

    static void destroyObject(EBObjectBase* object)
    {
        objectList.remove(object);
        delete object;
    }

    virtual void use(EBObjectPointerBase* w) = 0;
    virtual void unuse(EBObjectPointerBase* w) = 0;
    static inline int _counter = 0;

    /**
     * @brief Get the name
     *
     * @return std::string the generated name of the object
     **/
    virtual std::string getName()
    {
        return name;
    }

private:    
    static inline std::list<EBObjectBase*> objectList;
    static inline std::list<EBObjectBase*> objectToBeDestroyed;
    std::string name;
};

class EBObjectPointerBase
{
public:
    EBObjectPointerBase(EBObjectBase* pointer) : pointer(pointer)
    {
        if (pointer != nullptr)
            pointer->use(this);

        _counter++;
    }
    virtual ~EBObjectPointerBase()
    {
        if (pointer != nullptr)
            pointer->unuse(this);

        _counter--;
    }
    static inline int _counter = 0;

    bool isValid()
    {
        return pointer != nullptr;
    }

    void objectDeleted()
    {
        pointer = nullptr;
    }

    bool operator==(const EBObjectPointerBase& other)
    {
        return this->pointer == other.pointer;
    }

    bool operator!=(const EBObjectPointerBase& other)
    {
        return this->pointer != other.pointer;
    }

protected:
    EBObjectBase* pointer;
};

template <class T>
class EBObjectPointer : public EBObjectPointerBase
{
public:
    EBObjectPointer(EBObjectBase* pointer) : EBObjectPointerBase(pointer){}

    EBObjectPointer(const EBObjectPointer& other) : EBObjectPointerBase(other.pointer){}

    void operator=(const EBObjectPointer<T>& other)
    {
        if (this->pointer != nullptr)
            this->pointer->unuse(this);

        this->pointer = other.pointer;

        if (this->pointer != nullptr)
            this->pointer->use(this);
    }

    T* operator->()
    {
        if (pointer == nullptr)
            throw std::exception();

        return static_cast<T*>(pointer);
    }

    T* get() const
    {
        if (pointer == nullptr)
            throw std::exception();

        return static_cast<T*>(pointer);
    }
};

/**
 * @brief The core object. Every object from EBCpp must inherit this.
 *
 */
template <class T>
class EBObject : public EBObjectBase
{
public:
    /**
     * @brief Construct a new EBObject object
     *
     * @param parent The parent EBObject of this EBObject
     */
    EBObject() :
        EBObjectBase(std::string(typeid(this).name()) + " - " + EBUtils::intToHex(reinterpret_cast<long long>(this))),
        threadId(std::this_thread::get_id())
    {
    }

    /**
     * @brief Destroy the EBObject object
     *
     */
    virtual ~EBObject()
    {
        if (sharedPointer.size())
        {
            std::cout << "Object " << getName() << " killed while watched (" << std::dec << sharedPointer.size() << ")!" << std::endl;
            for (EBObjectPointerBase* w : sharedPointer)
            {
                w->objectDeleted();
            }
        }
    }

    EBObjectPointer<T> operator&()
    {
        return EBObjectPointer<T>(this);
    }

    template <class type>
    EBObjectPointer<type> cast()
    {
        return this;
    }

    virtual void use(EBObjectPointerBase* ptr)
    {
        mutex.lock();
        sharedPointer.push_back(ptr);
        mutex.unlock();
    }

    virtual void unuse(EBObjectPointerBase* ptr)
    {
        mutex.lock();
        {
            if (std::find(sharedPointer.begin(), sharedPointer.end(), ptr) != sharedPointer.end())
                sharedPointer.remove(ptr);
        }

        // Check if all references to a Heap object are gone
        if (sharedPointer.size() == 0)
        {
            if (std::find(objectList.begin(), objectList.end(), this) != objectList.end())
            {                
                objectList.remove(this);
                objectToBeDestroyed.push_back(this);
            }
        }
        mutex.unlock();
    }

    uint32_t watchCount()
    {
        return sharedPointer.size();
    }

private:
    std::list<EBObjectPointerBase*> sharedPointer;
    std::thread::id threadId;
    std::mutex mutex;
};

} // namespace EBCpp