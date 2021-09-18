cmake_minimum_required(VERSION 3.10)

project(EBCpp VERSION 0.1 LANGUAGES CXX)

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

# Compile EBCppExampleFile
add_executable(EBCppExample examples/Example.cpp)
if(WIN32)
    target_link_libraries(EBCppExample -Wl,-subsystem,console)
endif()

# Compile EBCppExampleFile
add_executable(EBCppExampleFile examples/ExampleFile.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleFile -Wl,-subsystem,console)
endif()

# Compile EBCppExampleTimer 
add_executable(EBCppExampleTimer examples/ExampleTimer.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleTimer -Wl,-subsystem,console)
endif()

# Compile EBCppExampleTcpClient 
add_executable(EBCppExampleTcpClient examples/ExampleTcpClient.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleTcpClient wsock32 ws2_32 -Wl,-subsystem,console)
endif()

# Compile EBCppExampleTcpServer 
add_executable(EBCppExampleTcpServer examples/ExampleTcpServer.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleTcpServer wsock32 ws2_32 -Wl,-subsystem,console)
endif()

# Compile EBCppExampleHttpServer 
add_executable(EBCppExampleHttpServer examples/ExampleHttpServer.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleHttpServer wsock32 ws2_32 -Wl,-subsystem,console)
endif()

# Compile EBCppExampleGui 
add_executable(EBCppExampleGui examples/ExampleGui.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleGui gdiplus -Wl,-subsystem,console)
endif()

# Compile EBCppExampleImage
add_executable(EBCppExampleGuiImage examples/ExampleGuiImage.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleGuiImage gdiplus -Wl,-subsystem,console)
endif()

# Compile EBCppExampleGuiLayout
add_executable(EBCppExampleGuiLayout examples/ExampleGuiLayout.cpp)
if(WIN32)
    target_link_libraries(EBCppExampleGuiLayout gdiplus -Wl,-subsystem,console)
endif()

find_package(OpenSSL)
if(OPENSSL_FOUND)    
    # Compile EBCppExampleSSLClient 
    add_executable(EBCppExampleSSLClient examples/ExampleSSLClient.cpp)
    target_include_directories(EBCppExampleSSLClient PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleSSLClient OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    endif()

    # Compile EBCppExampleSSLServer 
    add_executable(EBCppExampleSSLServer examples/ExampleSSLServer.cpp)
    target_include_directories(EBCppExampleSSLServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleSSLServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
    endif()    

    # Compile EBCppExampleHttpsServer 
    add_executable(EBCppExampleHttpsServer examples/ExampleHttpsServer.cpp)
    target_include_directories(EBCppExampleHttpsServer PRIVATE ${OPENSSL_INCLUDE_DIR})
    if(WIN32)
        target_link_libraries(EBCppExampleHttpsServer OpenSSL::Crypto OpenSSL::SSL wsock32 ws2_32 -Wl,-subsystem,console)
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