# EBCpp

[![CMake](https://github.com/Tropby/EBCpp/actions/workflows/cmake.yml/badge.svg)](https://github.com/Tropby/EBCpp/actions/workflows/cmake.yml)

Event Based C++ (EBCpp) will be a framework that uses Events (Signal-Slot) to provide features to the user. This framework is designed to work with C++17 and without a special pre compiler. The framework is a header only framework and designed that it could be used very easily. Just include the header files and use the functionalities provided.

The events are synchronised to one thread. Therefore the events are thread-safe. You can use multible threads by setting up additional `EBEventLoop`s. This should only be used by experienced users.

## Table of Content

* [Installation](#installation)
* [Example](#example)

## Installation

EBCpp is a header only framework. You include the "*.hpp" files and the system is working. Its that easy.

## Pointer

With EBCpp you should not use raw C pointer.  You should use `EBCpp::EBObjectBase::createObject<T>()`. The following example shows how to use the pointers.

```C++
#include "../src/EBObject.hpp"

using namespace EBCpp;

class Example : public EBObject<Example>
{
public:
    Example(int c) : c(c)
    {
    }

    void hello()
    {
        std::cout << "Hello :) c is " << c << std::endl;
    }

private:
    int c;
};

int main()
{
    {
        EBObjectPointer<Example> example = EBObjectBase::createObject<Example>(15);
        example->hello();
    }
    // Object will be destroyed at this point, because no more EBObjectPointers are pointing to it.
}

```

## Example

The following example shows the usage of an `EBTimer` that prints 5 messages and exists.

```C++
#include "../src/EBEventLoop.hpp"
#include "../src/EBTimer.hpp"
#include "../src/profile/EBLogger.hpp"
#include "../src/profile/EBProfile.hpp"

using namespace EBCpp;

class ExampleTimer : public EBObject<ExampleTimer>
{
public:
    ExampleTimer() 
    {
        timer.timeout.connect(this, &ExampleTimer::timeout);
        timer.start(1000);
    }

    ~ExampleTimer()
    {
        timer.timeout.disconnect(this, &ExampleTimer::timeout);
    }

private:
    EBTimer timer;

    EB_SLOT(timeout)
    {
        EB_PROFILE_FUNC();

        static int i = 0;
        EB_LOG("timeout " << ++i);

        if (i == 5)
            EBEventLoop::getInstance()->exit();
    }
};

int main()
{
    ExampleTimer exampleTimer;
    EBEventLoop::getInstance()->exec();
    return 0;
}
```

## GUI (Example)

Currently working on a GUI implementation. That can render a GUI for Windows (GDI+). Other renderes can be added.

GUI rendering is done in another thread than the event handler.

See ExampleGui.cpp for an example.
