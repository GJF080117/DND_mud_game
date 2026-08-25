cmake_minimum_required(VERSION 3.20)

project(DndMudDemo VERSION 0.1.0 LANGUAGES CXX)

# 只使用标准 C++17，避免依赖编译器扩展。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 将游戏主要功能放在一个公共库中，游戏程序和测试程序都可以使用。
add_library(dndmud_core
    src/Character.cpp
    src/CombatSystem.cpp
    src/CommandParser.cpp
    src/Game.cpp
    src/SaveService.cpp
    src/World.cpp
)

target_include_directories(dndmud_core PUBLIC include)

# 所有程序使用相同的警告等级和中文源码编码设置。
function(configure_dndmud_target target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

configure_dndmud_target(dndmud_core)

# 正式控制台游戏目标。
add_executable(dndmud_demo src/main.cpp)
target_link_libraries(dndmud_demo PRIVATE dndmud_core)
configure_dndmud_target(dndmud_demo)

include(CTest)
if(BUILD_TESTING)
    # 测试不依赖第三方框架，直接链接核心库。
    add_executable(dndmud_tests tests/test_main.cpp)
    target_link_libraries(dndmud_tests PRIVATE dndmud_core)
    configure_dndmud_target(dndmud_tests)
    add_test(NAME dndmud_tests COMMAND dndmud_tests)
endif()
