
# Require C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("EBCPP source dir: ${CMAKE_CURRENT_LIST_DIR}/src")

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

if(WIN32)

    # Add defines for windows
    set(WINDOWS_VERSION 0x0A00) 
    add_definitions(-DWINVER=${WINDOWS_VERSION})
    add_definitions(-D_WIN32_WINNT=${WINDOWS_VERSION})
    add_definitions(-DNTDDI_VERSION=0x06010000)

    if(EBCPP_USE_GUI)
        message("Using Windows GUI")
        set(EBCPP_LIBS -Wl,-subsystem,window)
    else()
        message("Using Windows Console")
        set(EBCPP_LIBS -Wl,-subsystem,console)
    endif()
endif()

if(EBCPP_STATIC)
    set(EBCPP_LIBS ${EBCPP_LIBS} -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)    
endif()

##########################
# OpenSSL
##########################

if(EBCPP_USE_SSL)
    message("EBCPP_USE_SSL => Try to find openssl!")

    # Set OpenSSL directory (for windows)
    set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/dep/openssl)
    message("Open ssl root directory: ${OPENSSL_ROOT_DIR}")

    if(EBCPP_USE_SSL_STATIC)
        message("EBCPP_USE_SSL_STATIC => ON")
        set(OPENSSL_USE_STATIC_LIBS TRUE)
    else()
        message("EBCPP_USE_SSL_STATIC => OFF")
    endif()

    find_package(OpenSSL)

    if(OPENSSL_FOUND)

        message("Used OpenSSL_VERSION: ${OPENSSL_VERSION}")
        message("Used OpenSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
        message("Used OpenSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")

        if(WIN32)
            set(EBCPP_LIBS ${EBCPP_LIBS} OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32)
        else()
            set(EBCPP_LIBS ${EBCPP_LIBS} OpenSSL::Crypto OpenSSL::SSL)
        endif()
    else()
        message(FATAL_ERROR "Can not find OpenSSL binaries. Have you compiled openssl for EBCpp?")
    endif()
else()
    message("EBCPP_USE_SSL => Off")
endif()


##########################
# SQLITE
##########################

if(EBCPP_USE_SQLITE)
    message("Try to find SQLite3")   

    find_package (SQLite3)
    if(SQLite3_FOUND)
        include_directories(${SQLite3_INCLUDE_DIRS})    
        set(EBCPP_LIBS ${EBCPP_LIBS} ${SQLite3_LIBRARIES})        
    else()
        if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/external/sqlite/sqlite3.c")
            add_library(SQLITE 
                ${CMAKE_CURRENT_LIST_DIR}/external/sqlite/sqlite3.c
            )
            set_target_properties(SQLITE PROPERTIES LINKER_LANGUAGE CXX)

            include_directories(${CMAKE_CURRENT_LIST_DIR}/external/sqlite)
            set(EBCPP_LIBS ${EBCPP_LIBS} SQLITE)
        else()
            message(FATAL_ERROR "Can not find SQLite. Did you install SQLite? See external/setupSQLite.bat for windows!")
        endif()
    endif()
else()
    message("EBCPP_USE_SQLITE => Off")
endif()