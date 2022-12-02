
cmake_minimum_required(VERSION 3.23)

project("AppMT_Tests" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

Include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.2.0
)

FetchContent_MakeAvailable(Catch2)

file(GLOB CORE "../*.cpp" "../*.hpp")

add_executable(AppMT_Tests ${CORE})

target_compile_definitions(AppMT_Tests PUBLIC 
    -DTESTING
)

target_link_libraries(AppMT_Tests PRIVATE Catch2::Catch2WithMain)

