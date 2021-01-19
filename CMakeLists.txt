cmake_minimum_required(VERSION 3.10)

project(EBCpp VERSION 1.0)

# Build EBCpp Examples
set(BUILD_EXAMPLES ON)
set(ENABLE_DEBBUNG_OUTPUT OFF)

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

if(${ENABLE_DEBBUNG_OUTPUT})
    add_compile_definitions(EB_ENABLE_DEBUGGING)
endif()

if(${BUILD_EXAMPLES})
    # Build Timer Example
        add_executable(EBCppExamplesTimer ${EBCppSources} ${EBCppExamples})
        target_compile_definitions(EBCppExamplesTimer PRIVATE USE_EXAMPLES)
        target_compile_definitions(EBCppExamplesTimer PRIVATE EXAMPLE_TIMER)
        if(WIN32)
            target_link_libraries(EBCppExamplesTimer wsock32 ws2_32 -Wl,-subsystem,console)
        endif()

    # Build Socket Example
        add_executable(EBCppExamplesSocket ${EBCppSources} ${EBCppExamples})
        target_compile_definitions(EBCppExamplesSocket PRIVATE USE_EXAMPLES)
        target_compile_definitions(EBCppExamplesSocket PRIVATE EXAMPLE_SOCKET)
        if(WIN32)
            target_link_libraries(EBCppExamplesSocket wsock32 ws2_32 -Wl,-subsystem,console)
        endif()

    # Build File Example
        add_executable(EBCppExamplesFile ${EBCppSources} ${EBCppExamples})
        target_compile_definitions(EBCppExamplesFile PRIVATE USE_EXAMPLES)
        target_compile_definitions(EBCppExamplesFile PRIVATE EXAMPLE_FILE)
            if(WIN32)
            target_link_libraries(EBCppExamplesFile wsock32 ws2_32 -Wl,-subsystem,console)
        endif()

    # Build Http Example
        add_executable(EBCppExamplesHttp ${EBCppSources} ${EBCppExamples})
        target_compile_definitions(EBCppExamplesHttp PRIVATE USE_EXAMPLES)
        target_compile_definitions(EBCppExamplesHttp PRIVATE EXAMPLE_HTTP)
            if(WIN32)
            target_link_libraries(EBCppExamplesHttp wsock32 ws2_32 -Wl,-subsystem,console)
        endif()        

    # Build Https Example
    find_package(OpenSSL REQUIRED)
    if(OPENSSL_FOUND)
        
        message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
        message("OpenSSL libraries: ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES}")
        
        add_executable(EBCppExamplesHttps ${EBCppSources} ${EBCppExamples}) 
        target_compile_definitions(EBCppExamplesHttps PRIVATE USE_EXAMPLES)
        target_compile_definitions(EBCppExamplesHttps PRIVATE EXAMPLE_HTTPS)
        target_compile_definitions(EBCppExamplesHttps PRIVATE EB_OPENSSL) 

        include_directories(${OPENSSL_INCLUDE_DIR})  
        if(WIN32)                        
            target_link_libraries(EBCppExamplesHttps OpenSSL::SSL OpenSSL::Crypto pthread wsock32 ws2_32 -Wl,-subsystem,console)
        endif()        
    endif()
endif()