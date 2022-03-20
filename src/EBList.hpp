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

#include "EBObject.hpp"

namespace EBCpp
{

template <typename T>
class EBListNode : public EBObject<EBListNode<T>>
{
public:
    EBListNode(const T& value) : value(value), next(nullptr), previous(nullptr)
    {
    }

    EBObjectPointer<EBListNode<T>>& getNext()
    {
        return next;
    }

    EBObjectPointer<EBListNode<T>>& getPrevious()
    {
        return previous;
    }

    void setNext(const EBObjectPointer<EBListNode<T>> next)
    {
        this->next = next;
    }

    void setPrevious(const EBObjectPointer<EBListNode<T>> previous)
    {
        this->previous = previous;
    }

    T get() 
    {
        return value;
    }

private:
    EBObjectPointer<EBListNode<T>> next;
    EBObjectPointer<EBListNode<T>> previous;
    T value;
};

template <typename T>
class EBListIterator : public EBObject<EBListIterator<T>>
{
public:
    EBListIterator(EBObjectPointer<EBListNode<T>> current) : current(current)
    {
    }

    EBListIterator(const EBListIterator& other) : current(other.current)
    {
    }

    const T get() const
    {
        return current->get();
    }

    const EBObjectPointer<EBListNode<T>>& getNode() const
    {
        return current;
    }

    EBCpp::EBListIterator<T> next()
    {
        if( current != nullptr )
            current = current->getNext();
        return *this;
    }

    bool operator==(const EBListIterator<T>& other)
    {
        return current == other.current;
    }

    bool operator!=(const EBListIterator<T>& other)
    {
        return current != other.current;
    }

    const EBListIterator<T> operator*() const
    {
        return current;
    }

private:
    EBObjectPointer<EBListNode<T>> current;
};

template <typename T>
class EBList : public EBObject<EBList<T>>
{
public:
    EBList() : _front(nullptr), _end(nullptr), current(nullptr), size(0)
    {
    }

    EBList(const EBList& other) : _front(nullptr), _end(nullptr), current(nullptr), size(0)
    {
        for( auto& i : other )
        {
            this->append(i.get());
        }        
    }

    T get(int index)
    {
        setCursor(index);
        return current->get();        
    }

    void setCursor(int index)
    {
        current = _front;
        while (index--)
        {
            current = current->getNext();
        }
    }

    bool contains( const T & item ) const
    {
        for( auto & it : *this )
        {
            if( it.get() == item )
            {
                return true;
            }
        }
        return false;
    }

    void insertAt(int index, T item)
    {
        if( index <= size )
        {
            EBObjectPointer<EBListNode<T>> lit = EBObjectBase::createObject<EBListNode<T>>(item);

            setCursor(index);
            if (current == nullptr)
            {
                _end->setNext(lit);
                lit->setPrevious(_end);
                current = lit;
                _end = lit;
                return;
            }

            EBObjectPointer<EBListNode<T>> next = nullptr;
            EBObjectPointer<EBListNode<T>> previous = nullptr;

            if( current != nullptr )
            {
                next = current->getNext();
                previous = current->getPrevious();
            }

            lit->setPrevious(previous);
            if (previous != nullptr)
                previous->setNext(lit);

            lit->setNext(current);

            if (current != nullptr)
                current->setPrevious(lit);

            if( current == _front )
                _front = lit;
            
            if( current == nullptr )
                _end = lit;

            current = lit;

            size++;
        }
    }

    void remove(T item)
    {
        for (auto& it : *this)
        {
            const T i = it.get();
            if ( item == i)
            {
                auto next = it.getNode()->getNext();
                auto previous = it.getNode()->getPrevious();

                if(next != nullptr)
                {
                    next->setPrevious(previous);
                }

                if (previous != nullptr)
                    previous->setNext(next);
                else
                    _front = next;

                size--;

                return;
            }
        }
    }

    void removeAt(int index)
    {
        setCursor(index);
        auto next = current->getNext();
        auto previous = current->getPrevious();

        if( current == _front )
            _front = next;

        if (current == _end)
            _end = previous;

        if (next != nullptr )
            next->setPrevious(previous);
        else
            next->setPrevious(nullptr);

        if( previous != nullptr )
            previous->setNext(next);
        else
            previous->setNext(nullptr);

        current = next;
        size--;
    }

    EBObjectPointer<EBListNode<T>> getFront() const
    {
        return _front;
    }

    EBObjectPointer<EBListNode<T>> getEnd() const
    {
        return _end;
    }

    void append(T item)
    {
        size++;
        if (_front == nullptr)
        {
            EBObjectPointer<EBListNode<T>> lit = EBObjectBase::createObject<EBListNode<T>>(item);
            _front = lit;
            _end = lit;
            current = lit;
        }
        else
        {
            EBObjectPointer<EBListNode<T>> lit = EBObjectBase::createObject<EBListNode<T>>(item);
            _end->setNext(lit);
            lit->setPrevious(_end);
            _end = lit;
            current = _end;
        }
    }

    int getSize() const
    {
        return size;
    }

    void clear()
    {
        while( size )
        {
            removeAt(0);
        }
    }

private:
    EBObjectPointer<EBListNode<T>> _front;
    EBObjectPointer<EBListNode<T>> _end;
    EBObjectPointer<EBListNode<T>> current;
    int size;
};

template <typename T>
inline EBCpp::EBListIterator<T>& operator++(EBCpp::EBListIterator<T>& a, int)
{
    a.next();
    return a;
}

template <typename T>
inline EBCpp::EBListIterator<T>& operator++(EBCpp::EBListIterator<T>& a)
{
    a.next();
    return a;    
}

template <typename T>
inline EBCpp::EBListIterator<T> begin(const EBCpp::EBList<T>& v)
{
    EBListIterator<T> it(v.getFront());
    return it;
}

template <typename T>
inline EBCpp::EBListIterator<T> end(const EBCpp::EBList<T>& v)
{
    EBListIterator<T> it(nullptr);
    return it;
}

} // namespace EBCpp
