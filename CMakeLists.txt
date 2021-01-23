cmake_minimum_required(VERSION 3.10)

project(EBCpp VERSION 1.0)

# Require C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
file(GLOB_RECURSE EBCppHeaders CONFIGURE_DEPENDS src/*.hpp src/**/*.hpp )
file(GLOB_RECURSE EBCppExamples CONFIGURE_DEPENDS examples/*.cpp examples/**/*.cpp )

# Add defines for windows
if(WIN32)
    set(WINDOWS_VERSION 0x0A00) 
    add_definitions(-DWINVER=${WINDOWS_VERSION})
    add_definitions(-D_WIN32_WINNT=${WINDOWS_VERSION})
endif()

add_executable(EBCpp src/main.cpp)

if(WIN32)
    target_link_libraries(EBCpp wsock32 ws2_32 -Wl,-subsystem,console)
endif()
