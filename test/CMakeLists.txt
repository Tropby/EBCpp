cmake_minimum_required(VERSION 3.10)

# EBString tests
add_executable(EBCppTestEBString EBString.cpp)
target_compile_options(EBCppTestEBString PRIVATE --coverage -O0)
target_link_libraries(EBCppTestEBString gcov)
if(WIN32)
else()
    target_link_libraries(EBCppTestEBString pthread)
endif()        

# EBList tests
add_executable(EBCppTestEBList EBList.cpp)
target_compile_options(EBCppTestEBList PRIVATE --coverage -O0)
target_link_libraries(EBCppTestEBList gcov)
if(WIN32)
else()
    target_link_libraries(EBCppTestEBList pthread)
endif()        

# EBTimer tests
add_executable(EBCppTestEBTimer EBTimer.cpp)
target_compile_options(EBCppTestEBTimer PRIVATE --coverage -O0)
target_link_libraries(EBCppTestEBTimer gcov)
if(WIN32)
else()
    target_link_libraries(EBCppTestEBTimer pthread)
endif()        

# EBUrl tests
add_executable(EBCppTestEBUrl EBUrl.cpp)
target_compile_options(EBCppTestEBUrl PRIVATE --coverage -O0)
target_link_libraries(EBCppTestEBUrl gcov)
if(WIN32)
else()
    target_link_libraries(EBCppTestEBUrl pthread)
endif()  

# EBList tests
add_executable(EBCppTestEBXml EBXml.cpp)
target_compile_options(EBCppTestEBList PRIVATE --coverage -O0)
target_link_libraries(EBCppTestEBList gcov)
if(WIN32)
else()
    target_link_libraries(EBCppTestEBList pthread)
endif()        
