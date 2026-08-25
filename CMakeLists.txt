cmake_minimum_required(VERSION 3.20)

project(DndMudDemo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dndmud_core
    src/Character.cpp
    src/CombatSystem.cpp
    src/CommandParser.cpp
    src/Game.cpp
    src/SaveService.cpp
    src/World.cpp
)

target_include_directories(dndmud_core PUBLIC include)

if(MSVC)
    target_compile_options(dndmud_core PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(dndmud_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(dndmud_demo src/main.cpp)
target_link_libraries(dndmud_demo PRIVATE dndmud_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(dndmud_tests tests/test_main.cpp)
    target_link_libraries(dndmud_tests PRIVATE dndmud_core)
    add_test(NAME dndmud_tests COMMAND dndmud_tests)
endif()
