/*
* EBCpp Project
* https://github.com/Tropby/EBCpp
* 
* ---
* 
* MIT License
* 
* Copyright (c) 2024 Carsten (Tropby)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
 *  Created on: 2021-01-23
 *      Author: Carsten (Tropby)
 */

#pragma once

#include "EBObject.hpp"

#define EBStringList EBCpp::EBList<EBCpp::EBString>

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

    int indexOf( const T & item ) const 
    {
        int i = 0; 
        for (auto& it : *this)
        {
            if (it.get() == item)
            {
                return i;
            }
            i++;
        }
        return -1;
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

        if( previous != nullptr )
            previous->setNext(next);

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
