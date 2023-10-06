cmake_minimum_required(VERSION 3.10)

project(EBCppExamples VERSION 0.1 LANGUAGES CXX)

# Require C++17 Standard
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Configure and Include EBCpp
set(EBCPP_USE_GUI On)           # On/Off - Using the windows gui functions (only for testing)
set(EBCPP_USE_SSL On)           # On/Off - Using openssl for ssl sockets and https
set(EBCPP_USE_SSL_STATIC On)    # On/Off - Activate static linking of openssl into your application (see openssl license)
set(EBCPP_USE_SQLITE On)        # On/Off - Using sqlite in your application (static linked)
set(EBCPP_STATIC On)            # On/Off - Static linking of libc, libc++ and winpthread

include(ebcpp.cmake)

# Compile EBCppExampleFile
add_executable(EBCppExample examples/Example.cpp)
if(WIN32)
    target_link_libraries(EBCppExample -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExample pthread)        
endif()

# Compile EBCppExcampleSignalSlot
add_executable(EBCppExampleSignalSlot examples/ExampleSignalSlot.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleSignalSlot -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleSignalSlot pthread)        
endif()

# Compile EBCppExampleFile
add_executable(EBCppExampleFile examples/ExampleFile.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleFile -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleFile pthread)        
endif()

# Compile EBCppExampleTimer 
add_executable(EBCppExampleTimer examples/ExampleTimer.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleTimer -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleTimer pthread)        
endif()

# Compile EBCppExampleTcpClient 
add_executable(EBCppExampleTcpClient examples/ExampleTcpClient.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleTcpClient wsock32 ws2_32 -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleTcpClient pthread)        
endif()

# Compile EBCppExampleTcpServer 
add_executable(EBCppExampleTcpServer examples/ExampleTcpServer.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleTcpServer wsock32 ws2_32 -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleTcpServer pthread)        
endif()

# Compile EBCppExampleHttpServer 
add_executable(EBCppExampleHttpServer examples/ExampleHttpServer.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleHttpServer wsock32 ws2_32 -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleHttpServer pthread)        
endif()

# Compile EBCppExampleHttpClient
add_executable(EBCppExampleHttpClient examples/ExampleHttpClient.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleHttpClient wsock32 ws2_32 -Wl,-subsystem,console)
else()
    target_link_libraries(EBCppExampleHttpClient pthread)        
endif()

if(WIN32)
    # Compile EBCppExampleHttpClient
    add_executable(EBCppExampleICMP examples/ExampleICMP.cpp)
    if(WIN32)
        target_link_libraries(EBCppExampleICMP iphlpapi wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleICMP pthread)        
    endif()

    # Compile EBCppExampleSerialPort
    add_executable(EBCppExampleSerialPort examples/ExampleSerialPort.cpp)
    if(WIN32)
        target_link_libraries(EBCppExampleSerialPort wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleSerialPort pthread)        
    endif()

    # Compile EBCppExampleGui 
    add_executable(EBCppExampleGui examples/ExampleGui.cpp)
    if(WIN32)
        target_link_libraries(EBCppExampleGui gdiplus -Wl,-subsystem,windows)
    else()
        target_link_libraries(EBCppExampleGui pthread)        
    endif()

    # Compile EBCppExampleImage
    add_executable(EBCppExampleGuiImage examples/ExampleGuiImage.cpp)
    if(WIN32)
        target_link_libraries(EBCppExampleGuiImage gdiplus -Wl,-subsystem,windows)
    else()
        target_link_libraries(EBCppExampleGui pthread)        
    endif()

    # Compile EBCppExampleGuiLayout
    add_executable(EBCppExampleGuiLayout examples/ExampleGuiLayout.cpp)
    if(WIN32)
        target_link_libraries(EBCppExampleGuiLayout gdiplus -Wl,-subsystem,windows)
    else()
        target_link_libraries(EBCppExampleGuiLayout pthread)        
    endif()    
endif()

find_package(OpenSSL)
if(OPENSSL_FOUND)    
    # Compile EBCppExampleHttpServer 
    add_executable(EBCppExampleWebSocketServer examples/ExampleWebSocketServer.cpp)
    target_include_directories(EBCppExampleWebSocketServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleWebSocketServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleWebSocketServer OpenSSL::Crypto OpenSSL::SSL pthread)        
    endif()

    # Compile EBCppExampleSSLClient 
    add_executable(EBCppExampleSSLClient examples/ExampleSSLClient.cpp)
    target_include_directories(EBCppExampleSSLClient PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleSSLClient OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleSSLClient OpenSSL::Crypto OpenSSL::SSL pthread)
    endif()

    # Compile EBCppExampleSSLServer 
    add_executable(EBCppExampleSSLServer examples/ExampleSSLServer.cpp)
    target_include_directories(EBCppExampleSSLServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleSSLServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleSSLServer OpenSSL::Crypto OpenSSL::SSL pthread)
    endif()    

    # Compile EBCppExampleHttpsServer 
    add_executable(EBCppExampleHttpsServer examples/ExampleHttpsServer.cpp)
    target_include_directories(EBCppExampleHttpsServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleHttpsServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleHttpsServer OpenSSL::Crypto OpenSSL::SSL pthread)
    endif()    
    
    # Compile EBCppExampleHttpClient
    add_executable(EBCppExampleHttpsClient examples/ExampleHttpsClient.cpp)
    if(WIN32)
        target_link_libraries(EBCppExampleHttpsClient OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    else()
        target_link_libraries(EBCppExampleHttpsClient OpenSSL::Crypto OpenSSL::SSL pthread)        
    endif()
        
endif()


add_subdirectory(test)

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if( DOXYGEN_FOUND )
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
    #set(DOXYGEN_PROJECT_LOGO "${CMAKE_SOURCE_DIR}/doc/images/o2_logo.png")
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../doc)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_DOT_NUM_THREADS 1)
    set(DOXYGEN_UML_LOOK YES)
    set(DOXYGEN_UML_LIMIT_NUM_FIELDS 50)
    set(DOXYGEN_TEMPLATE_RELATIONS YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_INTERACTIVE_SVG YES)
    set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
    set(DOXYGEN_DOT_TRANSPARENT YES)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_WARN_ID_UNDOCUMENTED NO)    

    set(DOXYGEN_PREDEFINED 
        "EB_SIGNAL(a) = EBCpp::EBEvent<> a"        
        "EB_SIGNAL_WITH_ARGS(a, b) = EBCpp::EBEvent<b> a"
        "EB_SLOT(a) = void a( EBObject * sender )"
        "EB_SLOT_WITH_ARGS(a, b) = void a( EBObject * sender, b )"
    )

    file(GLOB_RECURSE EBCppHeaders CONFIGURE_DEPENDS src/*.hpp src/**/*.hpp examples/*.cpp)

    doxygen_add_docs(doxygen 
        ${EBCppHeaders} README.md
        ALL
        COMMENT "Generating doxygen documentation for ${PROJECT_NAME}"
    )
else(DOXYGEN_FOUND)
    message("doxygen not found building without doxygen documentation") 
endif(DOXYGEN_FOUND)  