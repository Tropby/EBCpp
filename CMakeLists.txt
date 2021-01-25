cmake_minimum_required(VERSION 3.10)

project(EBCpp VERSION 0.1)

# Require C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Add defines for windows
if(WIN32)
    set(WINDOWS_VERSION 0x0A00) 
    add_definitions(-DWINVER=${WINDOWS_VERSION})
    add_definitions(-D_WIN32_WINNT=${WINDOWS_VERSION})
endif()

# Compile ExampleTimer 
add_executable(ExampleTimer examples/ExampleTimer.cpp)
if(WIN32)
    target_link_libraries(ExampleTimer -Wl,-subsystem,console)
endif()

# Compile ExampleTcpClient 
add_executable(ExampleTcpClient examples/ExampleTcpClient.cpp)
if(WIN32)
    target_link_libraries(ExampleTcpClient wsock32 ws2_32 -Wl,-subsystem,console)
endif()

# Compile ExampleTcpServer 
add_executable(ExampleTcpServer examples/ExampleTcpServer.cpp)
if(WIN32)
    target_link_libraries(ExampleTcpServer wsock32 ws2_32 -Wl,-subsystem,console)
endif()

# Compile ExampleHttpServer 
add_executable(ExampleHttpServer examples/ExampleHttpServer.cpp)
if(WIN32)
    target_link_libraries(ExampleHttpServer wsock32 ws2_32 -Wl,-subsystem,console)
endif()

find_package(OpenSSL)
if(OPENSSL_FOUND)    
    # Compile ExampleSSLClient 
    add_executable(ExampleSSLClient examples/ExampleSSLClient.cpp)
    target_include_directories(ExampleSSLClient PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(ExampleSSLClient OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    endif()

    # Compile ExampleSSLServer 
    add_executable(ExampleSSLServer examples/ExampleSSLServer.cpp)
    target_include_directories(ExampleSSLServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(ExampleSSLServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    endif()    

    # Compile ExampleHttpsServer 
    add_executable(ExampleHttpsServer examples/ExampleHttpsServer.cpp)
    target_include_directories(ExampleHttpsServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(ExampleHttpsServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    endif()        
endif()

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

