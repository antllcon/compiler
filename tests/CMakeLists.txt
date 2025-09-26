cmake_minimum_required(VERSION 3.28)
project(FiniteAutomationTests)

enable_testing()

add_executable(MachineTests
    MachineTests.cpp
)

target_link_libraries(MachineTests
    PRIVATE
        FiniteAutomation   
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(MachineTests)
