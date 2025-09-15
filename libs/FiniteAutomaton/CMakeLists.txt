cmake_minimum_required(VERSION 3.28.3)
project(FiniteAutomatonLibrary)

add_library(FiniteAutomaton STATIC)

target_sources(FiniteAutomaton PRIVATE
        src/MealyMachine.cpp
        src/MooreMachine.cpp
        src/Converter.cpp
)

target_include_directories(FiniteAutomaton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)