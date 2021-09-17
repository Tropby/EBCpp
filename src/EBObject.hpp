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

#include "EBException.hpp"
#include "EBUtils.hpp"

namespace EBCpp
{

class EBObjectPointerBase;
template <class T>
class EBObjectPointer;

/**
 * @brief Base class of EBCpp. Every class should inherit this class
 * 
 */
class EBObjectBase
{
public:
    /**
     * @brief Construct a new EBObjectBase object
     * 
     * @param name Name of the EBObject
     */
    EBObjectBase(std::string name) : name(name)
    {
        _counter++;
    }

    /**
     * @brief Destroy the EBObjectBase object
     * 
     */
    virtual ~EBObjectBase()
    {
        _counter--;    
    }

    /**
     * @brief Create a Object
     *
     *  Example:
     *      EBObjectPointer<Example> example = EBObjectBase::createObject<Example>(15);
     *
     * @tparam C Class name of the object that is created
     * @tparam argList Argument list of the constructor
     * @param args The arguments for the constructor
     * @return EBObjectPointer<C> Pointer to the created object.
     */
    template <class C, class... argList>
    static EBObjectPointer<C> createObject(argList... args)
    {
        C* object = new C(args...);
        objectList.push_back(object);
        return EBObjectPointer<C>(object);
    }

    /**
     * @brief Destroys all objects that are not used anymore
     *        Attention: do not call this! It is called by the
     *        event loop to destroy objects that are no longer used.
     *
     */
    static void destroyObjects()
    {
        for (EBObjectBase* ptr : objectToBeDestroyed)
            destroyObject(ptr);

        objectToBeDestroyed.clear();
    }

    /**
     * @brief Called by the pointer that are using this object
     * 
     * @param w Pointer that is unsing this object
     */
    virtual void use(EBObjectPointerBase* w) = 0;

    /**
     * @brief Called by the pointer that is no longer using this object
     * 
     * @param w Pointer that is no longer using this object
     */
    virtual void unuse(EBObjectPointerBase* w) = 0;

    /**
     * @brief Debug counter. Counting the current living EBObjects
     * 
     */
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
    /**
     * @brief Destroys a specific object
     *
     * @param object The pointer to the object to destroy
     */
    static void destroyObject(EBObjectBase* object)
    {
        objectList.remove(object);
        delete object;
    }

    static inline std::list<EBObjectBase*> objectList;
    static inline std::list<EBObjectBase*> objectToBeDestroyed;
    std::string name;
};

/**
 * @brief Class that provides pointer functionality to the EBObjectBase class
 * 
 */
class EBObjectPointerBase
{
public:
    /**
     * @brief Construct a new EBObjectPointerBase object
     * 
     * @param pointer The pointer to the EBObjectBase
     */
    EBObjectPointerBase(EBObjectBase* pointer) : pointer(pointer)
    {
        if (pointer != nullptr)
            pointer->use(this);

        _counter++;
    }

    /**
     * @brief Destroy the EBObjectPointerBase object
     * 
     */
    virtual ~EBObjectPointerBase()
    {
        if (pointer != nullptr)
            pointer->unuse(this);

        _counter--;
    }

    /**
     * @brief Pointer counter
     * 
     */
    static inline int _counter = 0;

    /**
     * @brief Checks if the pointer is valid.
     * 
     * @return true The pointer is valid an can be used
     * @return false The pointer is not valid
     */
    bool isValid()
    {
        return pointer != nullptr;
    }

    /**
     * @brief This method is called if the EBObject is deleted while the pointer is alive
     *        Info: This should never happen. But it can, if you delete objects by urself.
     * 
     */
    void objectDeleted()
    {
        pointer = nullptr;
    }

    /**
     * @brief 
     * 
     * @param other 
     * @return true 
     * @return false 
     */
    bool operator==(const EBObjectPointerBase& other)
    {
        return this->pointer == other.pointer;
    }

    /**
     * @brief 
     * 
     * @param other 
     * @return true 
     * @return false 
     */
    bool operator!=(const EBObjectPointerBase& other)
    {
        return this->pointer != other.pointer;
    }

protected:
    //! Pointer to the object that is observed by this pointer instance
    EBObjectBase* pointer;
};

/**
 * @brief Handles a pointer of a specific type 
 * 
 * @tparam T Type of the EBObjectBase that is handled by the pointer class
 */
template <class T>
class EBObjectPointer : public EBObjectPointerBase
{
public:
    /**
     * @brief Construct a new EBObjectPointer object
     *
     * @param pointer The pointer to the EBObjectBase
     */
    EBObjectPointer(EBObjectBase* pointer) : EBObjectPointerBase(pointer){}

    /**
     * @brief Construct a new EBObjectPointer object
     * 
     * @param other The EBObjectPointer that should be copied
     */
    EBObjectPointer(const EBObjectPointer& other) : EBObjectPointerBase(other.pointer){}

    /**
     * @brief Sets the pointer (this) to other.
     * 
     * @param other The pointer value that should be used
     */
    void operator=(const EBObjectPointer<T>& other)
    {
        if (this->pointer != nullptr)
            this->pointer->unuse(this);

        this->pointer = other.pointer;

        if (this->pointer != nullptr)
            this->pointer->use(this);
    }

    /**
     * @brief Get the raw pointer for the Object
     * 
     * @return T* Returns the raw pointer to the EBObjectBase
     */
    T* operator->()
    {
        if (pointer == nullptr)
            EB_EXCEPTION("Can not get pointer from a EBObjectPointer that is null!");

        return static_cast<T*>(pointer);
    }

    /**
     * @brief Get the raw pointer for the Object
     *
     * @return T* Returns the raw pointer to the EBObjectBase
     */
    T* get() const
    {
        if (pointer == nullptr)
            EB_EXCEPTION("Can not get pointer from a EBObjectPointer that is null!");

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

    /**
     * @brief Creates a managed pointer to the the EBObject of a specific type
     * 
     * @return EBObjectPointer<T> Pointer to this object
     */
    EBObjectPointer<T> operator&()
    {
        return EBObjectPointer<T>(this);
    }

    /**
     * @brief Cast the object to another pointer type
     * 
     * @tparam type Type of the object to point to
     * @return EBObjectPointer<type> The Pointer to the Object
     */
    template <class type>
    EBObjectPointer<type> cast()
    {
        return this;
    }

    /**
     * @brief Called by the EBObjectPointerBase if a new managed pointer is created.
     * 
     * @param ptr Pointer to the pointer object
     */
    virtual void use(EBObjectPointerBase* ptr)
    {
        mutex.lock();
        sharedPointer.push_back(ptr);
        mutex.unlock();
    }

    /**
     * @brief Called by the EBOBjectPointerBAse if a managed pointer is destroyed
     * 
     * @param ptr Pointer that is destroyed
     */
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

    /**
     * @brief Count of the current pointers that point to this object
     * 
     * @return uint32_t EBObjectPointer count that point to this object
     */
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