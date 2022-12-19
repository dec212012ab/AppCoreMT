cmake_minimum_required(VERSION 3.23)

project("AppCoreMT" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

if(MSVC)
    add_compile_options("/MP")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
endif()

#AppCoreMT
configure_file(src/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.hpp)

file(GLOB CORE "./src/*.cpp" "./src/*.hpp" "./src/interfaces/*")

add_library(objlib OBJECT ${CORE})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)
target_include_directories(objlib PRIVATE "./src/interfaces")

add_library(AppCoreMT_shared SHARED $<TARGET_OBJECTS:objlib>)
add_library(AppCoreMT_static STATIC $<TARGET_OBJECTS:objlib>)
#add_library(AppCoreMT $<TARGET_OBJECTS:objlib>)

#AppCoreMT_Tests
Include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.2.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(AppMT_Tests ${CORE})

target_compile_definitions(AppMT_Tests PUBLIC 
    -DTESTING
)

target_include_directories(AppMT_Tests PRIVATE "./src/interfaces")

target_link_libraries(AppMT_Tests PRIVATE Catch2::Catch2WithMain)

#Install command

file(GLOB install_includes "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
install(
    TARGETS AppCoreMT_static AppCoreMT_shared AppMT_Tests
    RUNTIME DESTINATION $<CONFIG>/bin
    LIBRARY DESTINATION $<CONFIG>/lib
    ARCHIVE DESTINATION $<CONFIG>/lib
)
install(FILES ${install_includes} DESTINATION include)