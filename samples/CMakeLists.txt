cmake_minimum_required(VERSION 3.23)
project("AppCoreMT_Sample" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_options("/MP")
endif()

add_executable(AppCoreMT_Sample src/main.cpp)
message(${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(
    AppCoreMT_Sample PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../install/include
)
if(MSVC)
    target_link_directories(
        AppCoreMT_Sample PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../install/$<CONFIG>/lib
    )
    target_link_libraries(
        AppCoreMT_Sample PUBLIC
        AppCoreMT_static.lib
    )
endif()
