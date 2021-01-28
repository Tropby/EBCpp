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

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>

namespace EBCpp
{

class EBObject;

/**
 * @brief The application class that keeps track of all EBObjects
 *
 */
class EBApplication
{
public:
    /**
     * @brief Get the Instance object
     *
     * @return EBApplication& Application instance
     */
    static EBApplication& getInstance()
    {
        static std::unique_ptr<EBApplication> instance;
        if (!instance)
        {
            instance = std::unique_ptr<EBApplication>(new EBApplication());
        }
        return *instance;
    }

    /**
     * @brief Get the infos about all objects
     *
     * @return std::string
     */
    static std::string getObjectsInfo()
    {
        EBApplication& app = EBApplication::getInstance();
        return "Living objects: " + std::to_string(app.livingObjects.size());
    }

    /**
     * @brief Called from each EBObject that is created
     *
     * @param object Object that is created
     */
    static void objectCreated(EBObject& object)
    {
        EBApplication& app = EBApplication::getInstance();

        app.mutex.lock();
        bool found =
        (std::find(app.livingObjects.begin(), app.livingObjects.end(), &object) != app.livingObjects.end());
        if (!found)
            app.livingObjects.push_back(&object);
        app.mutex.unlock();
    }

    /**
     * @brief Called from each EBObject that is destroyed
     *
     * @param object Object that is Destroyed
     */
    static void objectDestroyed(EBObject& object)
    {
        EBApplication& app = EBApplication::getInstance();

        app.mutex.lock();
        app.livingObjects.remove(&object);
        app.mutex.unlock();
    }

    /**
     * @brief Checks if a EBObject is valid and known to the EBApplication
     *
     * @param object Object that is checked
     * @return true if the object is valid
     * @return false if the object is unknwon
     */
    static bool isValidObject(EBObject& object)
    {
        return EBApplication::isValidObject(&object);
    }

    /**
     * @brief Checks if a EBObject is valid and known to the EBApplication
     *
     * @param object Object that is checked
     * @return true if the object is valid
     * @return false if the object is unknwon
     */
    static bool isValidObject(EBObject* object)
    {
        EBApplication& app = EBApplication::getInstance();
        return (std::find(app.livingObjects.begin(), app.livingObjects.end(), object) != app.livingObjects.end());
    }

private:
    EBApplication()
    {
    }

    std::list<EBObject*> livingObjects;
    std::mutex mutex;
};

} // namespace EBCpp