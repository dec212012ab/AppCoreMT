cmake_minimum_required(VERSION 3.23)
project("AppCoreMT_Sample" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_options("/MP")
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

#Executables
add_executable(00-TaskChain src/00-TaskChain/main.cpp)
add_executable(01-DLL_Loader src/01-DLL_Loader/main.cpp)
add_library(01-DLL SHARED src/../../install/include/IPlugin.hpp src/01-DLL_Loader/dll.cpp)



#message(${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(
    00-TaskChain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../install/include
)

target_include_directories(
    01-DLL_Loader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../install/include
)

target_include_directories(
    01-DLL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../install/include
)

if(MSVC)
    target_link_directories(
        00-TaskChain PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../install/$<CONFIG>/lib
    )
    target_link_libraries(
        00-TaskChain PUBLIC
        AppCoreMT_static.lib
    )

    target_link_directories(
        01-DLL_Loader PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../install/$<CONFIG>/lib
    )
    target_link_libraries(
        01-DLL_Loader PUBLIC
        AppCoreMT_static.lib
    )
endif()
