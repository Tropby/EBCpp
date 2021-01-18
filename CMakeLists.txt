cmake_minimum_required(VERSION 3.10)

# Build EBCpp Examples
set(BUILD_EXAMPLES ON)

# Require C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
file(GLOB_RECURSE EBCppSources CONFIGURE_DEPENDS src/*.cpp src/**/*.cpp )
file(GLOB_RECURSE EBCppHeaders CONFIGURE_DEPENDS src/*.h src/**/*.h )
file(GLOB_RECURSE EBCppExamples CONFIGURE_DEPENDS examples/*.cpp examples/**/*.cpp )

# Add defines for windows
if(WIN32)
    set(WINDOWS_VERSION 0x0A00)
    add_definitions(-DWINVER=${WINDOWS_VERSION})
    add_definitions(-D_WIN32_WINNT=${WINDOWS_VERSION})
endif()

if(${BUILD_EXAMPLES})
    # Build Timer Example
    project(EBCppExamplesTimer VERSION 1.0)
        add_compile_definitions(EBCppExamplesTimer USE_EXAMPLES)
        add_compile_definitions(EBCppExamplesTimer EXAMPLE_TIMER)

        add_executable(EBCppExamplesTimer ${EBCppSources} ${EBCppExamples} ${EBCppHeaders})
        if(WIN32)
            target_link_libraries(EBCppExamplesTimer wsock32 ws2_32 -Wl,-subsystem,console)
        endif()

    # Build Socket Example
    project(EBCppExamplesSocket VERSION 1.0)
        add_compile_definitions(EBCppExamplesSocket USE_EXAMPLES)
        add_compile_definitions(EBCppExamplesSocket EXAMPLE_TIMER)

        add_executable(EBCppExamplesSocket ${EBCppSources} ${EBCppExamples} ${EBCppHeaders})
        if(WIN32)
            target_link_libraries(EBCppExamplesSocket wsock32 ws2_32 -Wl,-subsystem,console)
        endif()
endif()