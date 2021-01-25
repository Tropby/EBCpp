/*
 * EBCpp
 *
 * Copyright (C) 2020 Carsten Grings
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

#include "EBApplication.hpp"

namespace EBCpp
{

/**
 * @brief The core object. Every object from EBCpp must inherit this.
 *
 */
class EBObject
{
public:
    /**
     * @brief Construct a new EBObject object
     *
     * @param parent The parent EBObject of this EBObject
     */
    EBObject(EBObject* parent) :
        name(std::string(typeid(this).name()) + " - " + std::to_string(reinterpret_cast<long long>(this))),
        threadId(std::this_thread::get_id()), parent(parent)
    {
        EBApplication::getInstance().objectCreated(*this);
        if (parent != nullptr)
            parent->registerChild(this);
    }

    /**
     * @brief Destroy the EBObject object
     *
     */
    ~EBObject()
    {
        EBApplication::getInstance().objectDestroyed(*this);
        if (parent != nullptr)
            parent->removeChild(this);

        // delete all child objects with the parent object
        while (childs.size())
        {
            EBObject* obj = childs.front();
            childs.pop_front();
            delete obj;
        }
    }

    /**
     * @brief Get the name
     *
     * @return std::string the generated name of the object
     **/
    std::string getName()
    {
        return name;
    }

private:
    std::string name;
    std::thread::id threadId;

    EBObject* parent;
    std::list<EBObject*> childs;

    void registerChild(EBObject* child)
    {
        childs.push_back(child);
    }

    void removeChild(EBObject* child)
    {
        childs.remove(child);
    }
};

} // namespace EBCpp