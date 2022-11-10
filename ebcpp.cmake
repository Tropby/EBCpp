
# Require C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("EBCPP source dir: ${CMAKE_CURRENT_LIST_DIR}/src")

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

if(WIN32)
    if(EBCPP_USE_GUI)
        message("Using Windows GUI")
        set(EBCPP_LIBS -Wl,-subsystem,window)
    else()
        message("Using Windows Console")
        set(EBCPP_LIBS -Wl,-subsystem,console)
    endif()
endif()

if(EBCPP_STATIC)
    set(EBCPP_LIBS ${EBCPP_LIBS} -static-libstdc++ -static-libgcc)    
endif()

if(EBCPP_USE_SSL)
    message("EBCPP_USE_SSL => Try to find openssl!")


    if(EBCPP_USE_SSL_STATIC)
        message("EBCPP_USE_SSL_STATIC => ON")
        set(OPENSSL_USE_STATIC_LIBS TRUE)
    else()
        message("EBCPP_USE_SSL_STATIC => OFF")
    endif()

    find_package(OpenSSL REQUIRED)

    message("Used OpenSSL_VERSION: ${OPENSSL_VERSION}")
    message("Used OpenSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
    message("Used OpenSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")

    if(WIN32)
        set(EBCPP_LIBS ${EBCPP_LIBS} OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32)
    else()
        set(EBCPP_LIBS ${EBCPP_LIBS} OpenSSL::Crypto OpenSSL::SSL)
    endif()
else()
    message("EBCPP_USE_SSL => Off")
endif()