# EBCpp

[![CMake on Windows (!SSL, !SQLITE)](https://github.com/Tropby/EBCpp/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/Tropby/EBCpp/actions/workflows/cmake-single-platform.yml)

Event Based C++ (EBCpp) will be a framework that uses Events (Signal-Slot) to provide features to the user. This framework is designed to work with C++17 and without a special pre compiler. The framework is a header only framework and designed that it could be used very easily. Just include the header files and use the functionalities provided.

The events are synchronised to one thread. Therefore the events are thread-safe. You can use multible threads by setting up additional `EBEventLoop`s. This should only be used by experienced users.

## Usage with CMake

Add EBCpp as submodule to your git project

```bash
mkdir dep
git submodule https://github.com/Tropby/EBCpp.git dep/ebcpp
```

Include EBCpp in your `CMakeLists.txt` file:

```cmake
# Configure and Include EBCpp
set(EBCPP_USE_GUI Off)          # On/Off - Using the windows gui functions (only for testing)
set(EBCPP_USE_SSL On)           # On/Off - Using openssl for ssl sockets and https
set(EBCPP_USE_SSL_STATIC On)    # On/Off - Activate static linking of openssl into your application (see openssl license)
set(EBCPP_USE_SQLITE On)        # On/Off - Using sqlite in your application (static linked)
set(EBCPP_STATIC On)            # On/Off - Static linking of libc, libc++ and winpthread

include(dep/ebcpp/ebcpp.cmake)
```

Now you can use EBCpp within your project.

### Using EBCpp the easy way

Create a `main.cpp` that includes `EBApplication.hpp`. The EBApplication provides a makro that creates the EBCpp init and shutdown functions and handels your main class. The follwoing code blocks are showing the usage of this makro.  

`CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.10)

project(Example)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Configure and Include EBCpp
set(EBCPP_USE_GUI Off)
set(EBCPP_USE_SSL Off)
set(EBCPP_USE_SSL_STATIC Off)
set(EBCPP_USE_SQLITE Off)
set(EBCPP_STATIC On)
include(dep/ebcpp/ebcpp.cmake)

# setup your application
add_executable(Example 
    main.cpp
    Example.cpp
)

# set ebcpp linking options to the target
target_link_libraries(Example ${EBCPP_LIBS})
```

`main.cpp`

```c++
// Include EBApplication and the main class header file
#include <EBApplication.hpp>
#include "Example.hpp"

// Setup main class (Example)
EB_APPLICATION(Example);
```

`Example.hpp`
```c++
#pragma once

#include <EBObject.hpp>

class Example : public EBCpp::EBObject<Example>
{
public:
    Example();
};
```

`Example.cpp`

```c++
#include "Example.hpp"

// Used for the logging output
#include <profile/EBLogger.hpp>

// Used to exit the event loop and shutdown the programm
#include <EBEventLoop.hpp>

TestBoe::TestBoe()
{
    // Output "Hellorld!"
    EB_LOG_INFO("Hellorld!");

    // Shutdown the programm
    EBCpp::EBEventLoop::getInstance()->exit();
}

```

## Examples

See examples direcory for more examples!

### Example Pointer

With EBCpp you should not use raw C pointer.  You should use `EBCpp::EBObjectBase::createObject<T>()`. The following example shows how to use the pointers. There is an `EBPtr` makro.

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

### Example with EBTimer

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
