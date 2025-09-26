cmake_minimum_required(VERSION 3.28.3)
project("FiniteAutomation")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(libs/FiniteAutomation)
add_executable(start main.cpp)
target_link_libraries(start PRIVATE FiniteAutomation)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
FetchContent_MakeAvailable(googletest)