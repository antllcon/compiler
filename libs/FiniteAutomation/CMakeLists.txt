cmake_minimum_required(VERSION 3.28.3)
project(FiniteAutomationLibrary)

add_library(FiniteAutomation STATIC
    src/MealyMachine.cpp
    src/MooreMachine.cpp
)

target_include_directories(FiniteAutomation 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

